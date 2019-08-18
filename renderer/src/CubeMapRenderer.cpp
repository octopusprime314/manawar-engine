#include "IOEventDistributor.h"
#include "CubeMapRenderer.h"

CubeMapRenderer::CubeMapRenderer(GLuint width,
                                 GLuint height,
                                 bool   isDepth) :
    _cubeTextureMap(width,
                    height,
                    isDepth),
    _isDepth(       isDepth) {
}

CubeMapRenderer::~CubeMapRenderer() {

}

Texture* CubeMapRenderer::getCubeMapTexture() {
    return &_cubeTextureMap;
}

void CubeMapRenderer::preCubeFaceRender(std::vector<Entity*> entityList,
                                        MVP*                 mvp) {

    //Need to change viewport to the resolution of the shadow texture
    glViewport(0,
               0,
               _cubeTextureMap.getWidth(),
               _cubeTextureMap.getHeight());

    //Bind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, _cubeTextureMap.getCubeFrameBufferContext());

    //Clear buffer
    if (_isDepth) {
        GLenum buffers[] = { GL_NONE };
        glClear(GL_DEPTH_BUFFER_BIT);
        glDrawBuffers(1, buffers);
    }
    else {
        GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawBuffers(1, buffers);
    }

    _transforms.clear();
    float* model    = mvp->getModelMatrix().getFlatBuffer();
    Matrix position = Matrix::translation(-model[3],
                                          -model[7],
                                          -model[11]);
    Matrix proj     = mvp->getProjectionMatrix();

    //Looking in +x direction
    //Need to rotate 180 degrees around z axis to align cube map texture correctly
    _transforms.push_back(proj                            *
                          Matrix::rotationAroundZ(180.0f) *
                          Matrix::rotationAroundY(90.0f)  *
                          position);
    //Looking in -x direction
    //Need to rotate 180 degrees around z axis to align cube map texture correctly
    _transforms.push_back(proj                            *
                          Matrix::rotationAroundZ(180.0f) *
                          Matrix::rotationAroundY(-90.0f) *
                          position);
    //Looking in +y direction
    _transforms.push_back(proj                            *
                          Matrix::rotationAroundX(-90.0f) *
                          position);
    //Looking in -y direction
    _transforms.push_back(proj                            *
                          Matrix::rotationAroundX(90.0f)  *
                          position);
    //Looking in +z direction
    //Need to rotate 180 degrees around z axis to align cube map texture correctly
    _transforms.push_back(proj                            *
                          Matrix::rotationAroundZ(180.0f) *
                          Matrix::rotationAroundY(180.0f) *
                          position);
    //Looking in -z direction
    //Need to rotate 180 degrees around z axis to align cube map texture correctly
    _transforms.push_back(proj                            *
                          Matrix::rotationAroundZ(180.0f) *
                          position);
}

void CubeMapRenderer::postCubeFaceRender() {
    //remove framebuffer context
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Need to change viewport to the resolution of the window size
    glViewport(0,
               0,
               IOEventDistributor::screenPixelWidth,
               IOEventDistributor::screenPixelHeight);
}
