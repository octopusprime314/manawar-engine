#include "Picker.h"
#include "IOEvents.h"
#include "EngineManager.h"
#include "IOEventDistributor.h"

Picker::Picker(MRTFrameBuffer*                    mrt,
               std::function<bool(Vector4, bool)> terminalCallback,
               std::function<bool(Entity*)>       mouseDeleteCallback) :
               _mouseDeleteCallback(mouseDeleteCallback),
               _mouseCallback(terminalCallback),
               _leftMousePressed(false),
               _idBufferCache(nullptr),
               _textureSelection(0),
               _pickingRadius(0),
               _mrt(mrt) {

    IOEvents::subscribeToMouseClick(std::bind(&Picker::_mouseClick,    this, _1, _2, _3, _4));
    IOEvents::subscribeToMouse(     std::bind(&Picker::_mouseMove,     this, _1, _2        ));
    IOEvents::subscribeToKeyboard(  std::bind(&Picker::_keyboardPress, this, _1, _2, _3    ));
}

Picker::~Picker() {

}

void Picker::updateIdBuffer() {

    int packAlignment;
    int w;
    int h;

    glBindTexture(           GL_TEXTURE_2D, _mrt->getTextureContexts()[2]);
    glGetIntegerv(           GL_PACK_ALIGNMENT, &packAlignment);

    glGetTexLevelParameteriv(GL_TEXTURE_2D,
                             0,
                             GL_TEXTURE_WIDTH,
                             &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D,
                             0,
                             GL_TEXTURE_HEIGHT,
                             &h);
 
    unsigned int size  = w * h * packAlignment;

    if (_idBufferCache == nullptr) {
        _idBufferCache = new float[size];
    }

    glGetTexImage(           GL_TEXTURE_2D,
                             0,
                             GL_RGBA,
                             GL_FLOAT,
                             reinterpret_cast<void*>(_idBufferCache));

}

void Picker::_keyboardPress(int key,
                            int x,
                            int y) {

    if (key == GLFW_KEY_UP) {
        _pickingRadius++;
    }
    else if (key == GLFW_KEY_DOWN) {
        if (_pickingRadius > 0) {
            _pickingRadius--;
        }
    }
}

Vector4 Picker::getLastPickedPosition() {
    return _pickedPosition;
}

void Picker::saveMutableTextures() {

    for (auto mutableTexture : _mutableTextureCache) {
        mutableTexture.second->saveToDisk();
    }
}


void Picker::_editData(int x, int y, bool mouseDrag, bool mouseClick) {
    
    int packAlignment;
    int w;
    int h;

    if (_idBufferCache == nullptr) {
        return;
    }
    glBindTexture(GL_TEXTURE_2D,     _mrt->getTextureContexts()[2]);
    glGetIntegerv(GL_PACK_ALIGNMENT, &packAlignment);

    glGetTexLevelParameteriv(GL_TEXTURE_2D,
                             0,
                             GL_TEXTURE_WIDTH,
                             &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D,
                             0,
                             GL_TEXTURE_HEIGHT,
                             &h);

    //invert y mouse position
    y = h - y;

    if (x <= 0 || x >= IOEventDistributor::screenPixelWidth ||
        y <= 0 || y >= IOEventDistributor::screenPixelHeight) {
        return;
    }

    unsigned int entityID = static_cast<unsigned int>(
        _idBufferCache[((x * packAlignment) + (y * w * packAlignment)) + 2] * 16777216.0f);

    unsigned int triangleID = static_cast<unsigned int>(
        _idBufferCache[((x * packAlignment) + (y * w * packAlignment)) + 3] * 16777216.0f);

    auto entityList = *EngineManager::getEntityList();

    for (auto entity : entityList) {
        entity->setSelected(true);
        if (entity->getID() == entityID &&
            entity->getModel()->getRenderBuffers() != nullptr) {

            //Make first call back to see if this is a mouse deletion
            if (mouseClick &&
                _mouseDeleteCallback(entity)) {

                break;
            }
            if (entity->getLayeredTexture() != nullptr) {
                auto layeredTexture = entity->getLayeredTexture()->getTextures();
                //used to retrieve triangle in entity
                auto renderBuffers = entity->getModel()->getRenderBuffers();
                std::string layeredTextureName = "Layered";
                for (auto& texture : layeredTexture) {
                    layeredTextureName += texture->getName();
                }

                auto texture = TextureBroker::instance()->getLayeredTexture(layeredTextureName);
                if (texture != nullptr) {
                    auto layeredTextures = texture->getTextures();
                    for (auto texture : layeredTextures) {
                        if (texture->getName().find("alphamap") != std::string::npos) {
                            Vector4 tileExtents;
                            std::string extentsInString = texture->getName().substr(texture->getName().find("clone") + 5);
                            int xPosOfTile              = std::atoi(extentsInString.substr(0, extentsInString.find("_")).c_str());
                            extentsInString             = extentsInString.substr(extentsInString.find("_") + 1);
                            int yPosOfTile              = std::atoi(extentsInString.substr(0, extentsInString.find("_")).c_str());
                            extentsInString             = extentsInString.substr(extentsInString.find("_") + 1);
                            int zPosOfTile              = std::atoi(extentsInString.substr(0, extentsInString.find(".")).c_str());

                            auto vertices               = renderBuffers->getVertices();
                            Vector4 A                   = entity->getWorldSpaceTransform() * (*vertices)[triangleID * 3];
                            Vector4 B                   = entity->getWorldSpaceTransform() * (*vertices)[(triangleID * 3) + 1];
                            Vector4 C                   = entity->getWorldSpaceTransform() * (*vertices)[(triangleID * 3) + 2];


                            MutableTexture* alphaMapEditor = nullptr;
                            if (_mutableTextureCache.find(texture->getName()) != _mutableTextureCache.end()) {
                                alphaMapEditor = _mutableTextureCache.find(texture->getName())->second;
                            }
                            else {
                                alphaMapEditor = new MutableTexture(texture->getName());
                                _mutableTextureCache.insert(std::pair<std::string, MutableTexture*>(texture->getName(), alphaMapEditor));
                            }
                            float width       = static_cast<float>(alphaMapEditor->getWidth());
                            float height      = static_cast<float>(alphaMapEditor->getHeight());
                                              
                            float xOffset     = width  / 2.0f;
                            float zOffset     = height / 2.0f;
                                              
                            float xCentroid   = ((A.getx() + B.getx() + C.getx()) / 3.0f);
                            float zCentroid   = ((A.getz() + B.getz() + C.getz()) / 3.0f);
                            float totalRadius = (width / 2) + _pickingRadius;

                            if (xCentroid >= xPosOfTile - totalRadius &&
                                xCentroid <= xPosOfTile + totalRadius &&
                                zCentroid >= zPosOfTile - totalRadius &&
                                zCentroid <= zPosOfTile + totalRadius) {

                                if (xCentroid < xOffset) {
                                    float multiplier = ceilf(fabs(xCentroid) / width);
                                    xCentroid        += multiplier * width;
                                }

                                if (zCentroid < zOffset) {
                                    float multiplier = ceilf(fabs(zCentroid) / height);
                                    zCentroid        += multiplier * height;
                                }

                                int xPosition   = static_cast<int>(xCentroid + xOffset) % static_cast<int>(width);
                                int zPosition   = static_cast<int>(zCentroid + zOffset) % static_cast<int>(height);
                                _pickedPosition = A;

                                bool modelUpdate = _mouseCallback(A, mouseClick);

                                if (modelUpdate == false) {
                                    alphaMapEditor->editTextureData(xPosition,
                                                                    zPosition,
                                                                    _pixelEditValue,
                                                                    (mouseDrag == true && mouseClick == false),
                                                                    _pickingRadius);
                                }
                            }
                        }
                    }
                }
            }
        }
        else {
            entity->setSelected(false);
        }
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Picker::_mouseMove(double x, double y) {
    
    if (_leftMousePressed) {
        _editData(static_cast<int>(x), static_cast<int>(y), true, true);
    }
    if (_leftMousePressed == false) {
        _editData(static_cast<int>(x), static_cast<int>(y), true, false);
    }

    _mousePosition = Vector4(static_cast<float>(x), static_cast<float>(y), 0.0f);
}

void Picker::_mouseClick(int button, int action, int x, int y) {

    //keep track of mouse pressed or released state for dragging
    if (button == GLFW_MOUSE_BUTTON_LEFT &&
        x      >= 0                      &&
        y      >= 0) {

        if (action == GLFW_PRESS) {
            _leftMousePressed = true;
        }
        else if (action == GLFW_RELEASE) {
            //Release typically means an action has been done so update the id buffer
            updateIdBuffer();
            _leftMousePressed = false;
        }
    }

    if (button == GLFW_MOUSE_BUTTON_MIDDLE &&
        action == GLFW_PRESS) {

        if (_textureSelection == 0) {
            _pixelEditValue = Vector4(255, 0, 0, 255);
        }
        else if (_textureSelection == 1) {
            _pixelEditValue = Vector4(0, 255, 0, 255);
        }
        else if (_textureSelection == 2) {
            _pixelEditValue = Vector4(0, 0, 255, 255);
        }
        else if (_textureSelection == 3) {
            _pixelEditValue = Vector4(0, 0, 0, 0); //inverse for alpha value
        }
        _editData(x, y, true, false);
        _textureSelection++;
        _textureSelection %= 4;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT &&
             action == GLFW_PRESS             &&
             x >= 0                           &&
             y >= 0) {
        _editData(x, y, false, true);
    }
    _mousePosition = Vector4(static_cast<float>(x), static_cast<float>(y), 0.0f);
}
