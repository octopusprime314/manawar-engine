#include "SkyBox.h"
#include "Model.h"

SkyBox::SkyBox(std::string folder) :
    _skyboxShader("skyboxShader") {

    TextureBroker* textureManager = TextureBroker::instance();

    textureManager->addCubeTexture(TEXTURE_LOCATION + folder);

    _cubeMapTexture = textureManager->getTexture(TEXTURE_LOCATION + folder);
    
    RenderBuffers renderBuffers;

    renderBuffers.addVertex(Vector4(-1.0f, 1.0f, -1.0f));
    renderBuffers.addVertex(Vector4(-1.0f, -1.0f, -1.0f));
    renderBuffers.addVertex(Vector4(1.0f, -1.0f, -1.0f));
    renderBuffers.addVertex(Vector4(1.0f, -1.0f, -1.0f));
    renderBuffers.addVertex(Vector4(1.0f, 1.0f, -1.0f));
    renderBuffers.addVertex(Vector4(-1.0f, 1.0f, -1.0f));

    renderBuffers.addVertex(Vector4(-1.0f, -1.0f, 1.0f));
    renderBuffers.addVertex(Vector4(-1.0f, -1.0f, -1.0f));
    renderBuffers.addVertex(Vector4(-1.0f, 1.0f, -1.0f));
    renderBuffers.addVertex(Vector4(-1.0f, 1.0f, -1.0f));
    renderBuffers.addVertex(Vector4(-1.0f, 1.0f, 1.0f));
    renderBuffers.addVertex(Vector4(-1.0f, -1.0f, 1.0f));

    renderBuffers.addVertex(Vector4(1.0f, -1.0f, -1.0f));
    renderBuffers.addVertex(Vector4(1.0f, -1.0f, 1.0f));
    renderBuffers.addVertex(Vector4(1.0f, 1.0f, 1.0f));
    renderBuffers.addVertex(Vector4(1.0f, 1.0f, 1.0f));
    renderBuffers.addVertex(Vector4(1.0f, 1.0f, -1.0f));
    renderBuffers.addVertex(Vector4(1.0f, -1.0f, -1.0f));

    renderBuffers.addVertex(Vector4(-1.0f, -1.0f, 1.0f));
    renderBuffers.addVertex(Vector4(-1.0f, 1.0f, 1.0f));
    renderBuffers.addVertex(Vector4(1.0f, 1.0f, 1.0f));
    renderBuffers.addVertex(Vector4(1.0f, 1.0f, 1.0f));
    renderBuffers.addVertex(Vector4(1.0f, -1.0f, 1.0f));
    renderBuffers.addVertex(Vector4(-1.0f, -1.0f, 1.0f));

    renderBuffers.addVertex(Vector4(-1.0f, 1.0f, -1.0f));
    renderBuffers.addVertex(Vector4(1.0f, 1.0f, -1.0f));
    renderBuffers.addVertex(Vector4(1.0f, 1.0f, 1.0f));
    renderBuffers.addVertex(Vector4(1.0f, 1.0f, 1.0f));
    renderBuffers.addVertex(Vector4(-1.0f, 1.0f, 1.0f));
    renderBuffers.addVertex(Vector4(-1.0f, 1.0f, -1.0f));

    renderBuffers.addVertex(Vector4(-1.0f, -1.0f, -1.0f));
    renderBuffers.addVertex(Vector4(-1.0f, -1.0f, 1.0f));
    renderBuffers.addVertex(Vector4(1.0f, -1.0f, -1.0f));
    renderBuffers.addVertex(Vector4(1.0f, -1.0f, -1.0f));
    renderBuffers.addVertex(Vector4(-1.0f, -1.0f, 1.0f));
    renderBuffers.addVertex(Vector4(1.0f, -1.0f, 1.0f));

    _vbo.createVBO(&renderBuffers, ModelClass::ModelType);

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