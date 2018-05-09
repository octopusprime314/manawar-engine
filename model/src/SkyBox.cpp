#include "SkyBox.h"
#include "Model.h"

SkyBox::SkyBox(std::string folder, ViewManagerEvents* eventWrapper) : 
    UpdateInterface(eventWrapper),
    _skyboxShader("skyboxShader") {

    TextureBroker* textureManager = TextureBroker::instance();

    textureManager->addCubeTexture(TEXTURE_LOCATION + folder);

    _cubeMapTexture = textureManager->getTexture(TEXTURE_LOCATION + folder);
}

SkyBox::~SkyBox() {

}

MVP* SkyBox::getMVP() {
    return &_mvp;
}
VBO* SkyBox::getVBO() {
    return &_vbo;
}

Texture* SkyBox::getCubeMapTexture() {
    return _cubeMapTexture;
}

void SkyBox::render() {
    _skyboxShader.runShader(this);
}

void SkyBox::_updateDraw() {
}
void SkyBox::_updateKeyboard(int key, int x, int y) {
}
void SkyBox::_updateReleaseKeyboard(int key, int x, int y) {
}
void SkyBox::_updateMouse(double x, double y) {
}

void SkyBox::_updateView(Matrix view) {

    _mvp.setView(view); //Receive updates when the view matrix has changed

     //If view changes then change our normal matrix
    _mvp.setNormal(view.inverse().transpose());
}

void SkyBox::_updateProjection(Matrix projection) {
    _mvp.setProjection(projection); //Receive updates when the projection matrix has changed
}