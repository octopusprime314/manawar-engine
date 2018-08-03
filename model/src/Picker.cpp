#include "Picker.h"
#include "SimpleContextEvents.h"

Picker::Picker(MRTFrameBuffer* mrt) : _mrt(mrt){
    SimpleContextEvents::subscribeToMouseClick(std::bind(&Picker::_mouseClick, this, _1, _2, _3, _4));
}

Picker::~Picker() {

}

void Picker::addPickableEntities(std::vector<Entity*> entities) {
    _entityList = entities;
}

void Picker::_mouseClick(int button, int action, int x, int y) {

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        std::cout << "Cursor Position at (" << x << " : " << y << std::endl;

        glBindTexture(GL_TEXTURE_2D, _mrt->getTextureContexts()[4]);
        int packAlignment;
        glGetIntegerv(GL_PACK_ALIGNMENT, &packAlignment);

        int format;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
        int w;
        int h;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

        unsigned int size = w * h * sizeof(unsigned char) * packAlignment;
        unsigned char *pixels = new unsigned char[size];
        glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, pixels);

        //invert y mouse position
        y = h - y;
        unsigned int entityID = static_cast<unsigned int>(pixels[(x * packAlignment) + (y * w * packAlignment)]);
        delete[] pixels;

        for (auto entity : _entityList) {
            if (entity->getID() == entityID) {
                entity->setSelected(true);
            }
            else {
                entity->setSelected(false);
            }
        }

        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
