#include "Picker.h"
#include "SimpleContextEvents.h"

Picker::Picker(MRTFrameBuffer* mrt, std::function<void(Vector4)> terminalCallback) :
    _mrt(mrt),
    _textureSelection(0),
    _mouseCallback(terminalCallback) {
    SimpleContextEvents::subscribeToMouseClick(std::bind(&Picker::_mouseClick, this, _1, _2, _3, _4));
}

Picker::~Picker() {

}

void Picker::addPickableEntities(std::vector<Entity*> entities) {
    _entityList = entities;
}

void Picker::_mouseClick(int button, int action, int x, int y) {

    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
        
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
        _textureSelection++;
        _textureSelection %= 4;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS &&
        x >= 0 && y >= 0) {

        glBindTexture(GL_TEXTURE_2D, _mrt->getTextureContexts()[2]);
        int packAlignment;
        glGetIntegerv(GL_PACK_ALIGNMENT, &packAlignment);

        int w;
        int h;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

        unsigned int size = w * h * packAlignment;
        float *pixels = new float[size];

        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, reinterpret_cast<void*>(pixels));

        //invert y mouse position
        y = h - y;

        unsigned int entityID = static_cast<unsigned int>(
            pixels[((x * packAlignment) + (y * w * packAlignment)) + 2] * 16777216.0f);

        unsigned int triangleID = static_cast<unsigned int>(
            pixels[((x * packAlignment) + (y * w * packAlignment)) + 3] * 16777216.0f);

        delete[] pixels;

        Entity* selectedEntity = nullptr;
        for (auto entity : _entityList) {
            if (entity->isID(entityID) && entityID > 0) {
                entity->setSelected(true);
                if (entity->getRenderBuffers()->size() > 0) {

                    selectedEntity = entity;

                    //used to retrieve triangle in entity
                    auto renderBuffers = entity->getRenderBuffers();
                    auto renderBuffer = (*renderBuffers)[entityID - 1];

                    auto textureIndex = (*(*renderBuffers)[entityID - 1].getTextureMapIndices())[triangleID];
                    std::string textureName = (*(*renderBuffers)[entityID - 1].getTextureMapNames())[textureIndex];

                    auto vertices = renderBuffer.getVertices();
                    Vector4 A = (*vertices)[triangleID * 3];
                    Vector4 B = (*vertices)[(triangleID * 3) + 1];
                    Vector4 C = (*vertices)[(triangleID * 3) + 2];

                    if (selectedEntity != nullptr) {

                        auto texture = TextureBroker::instance()->getLayeredTexture(textureName);
                        auto layeredTextures = texture->getTextures();
                        for (auto texture : layeredTextures) {
                            if (texture->getName().find("alphamap") != std::string::npos) {
                                _alphaMapEditor = new MutableTexture(texture->getName());
                                float width = static_cast<float>(_alphaMapEditor->getWidth());
                                float height = static_cast<float>(_alphaMapEditor->getHeight());

                                float xOffset = width / 2.0f;
                                float zOffset = height / 2.0f;

                                float xCentroid = ((A.getx() + B.getx() + C.getx()) / 3.0f);
                                float zCentroid = ((A.getz() + B.getz() + C.getz()) / 3.0f);

                                if (xCentroid < xOffset) {
                                    float multiplier = ceilf(fabs(xCentroid) / width);
                                    xCentroid += multiplier * width;
                                }

                                if (zCentroid < zOffset) {
                                    float multiplier = ceilf(fabs(zCentroid) / height);
                                    zCentroid += multiplier * height;
                                }

                                int xPosition = static_cast<int>(xCentroid + xOffset) % static_cast<int>(width);
                                int zPosition = static_cast<int>(zCentroid + zOffset) % static_cast<int>(height);

                                _mouseCallback(A);

                                _alphaMapEditor->editTextureData(xPosition, zPosition, _pixelEditValue);
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
}
