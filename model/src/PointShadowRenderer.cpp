#include "PointShadowRenderer.h"

PointShadowRenderer::PointShadowRenderer(GLuint width, GLuint height) :
    _pointShadowShader("pointShadowShader"),
    _pointAnimatedShadowShader("pointAnimatedShadowShader"),
    _cubeTextureMap(width, height) { 

}

PointShadowRenderer::~PointShadowRenderer() {

}

GLuint PointShadowRenderer::getCubeMapDepthTexture() {
    return _cubeTextureMap.getCubeDepthMapContext();
}

void PointShadowRenderer::generateShadowBuffer(std::vector<Model*> modelList, std::vector<Light*>& lights) {

    for (Light* light : lights) {

        if (light->getType() == LightType::POINT) {
            std::vector<Matrix> lightTransforms;
            Matrix lightTrans = Matrix::cameraTranslation(light->getPosition().getx(),
                light->getPosition().gety(), light->getPosition().getz());

            Matrix lightProj = light->getMVP().getProjectionMatrix();

            //Looking in +x direction
            //Need to rotate 180 degrees around z axis to align cube map texture correctly
            lightTransforms.push_back(lightProj * Matrix::rotationAroundZ(180.0f) * Matrix::rotationAroundY(90.0f) * lightTrans);
            //Looking in -x direction
            //Need to rotate 180 degrees around z axis to align cube map texture correctly
            lightTransforms.push_back(lightProj * Matrix::rotationAroundZ(180.0f) * Matrix::rotationAroundY(-90.0f) * lightTrans);
            //Looking in +y direction
            lightTransforms.push_back(lightProj * Matrix::rotationAroundX(-90.0f) * lightTrans);
            //Looking in -y direction
            lightTransforms.push_back(lightProj * Matrix::rotationAroundX(90.0f) * lightTrans);
            //Looking in +z direction
            //Need to rotate 180 degrees around z axis to align cube map texture correctly
            lightTransforms.push_back(lightProj * Matrix::rotationAroundZ(180.0f) * Matrix::rotationAroundY(180.0f) * lightTrans);
            //Looking in -z direction
            //Need to rotate 180 degrees around z axis to align cube map texture correctly
            lightTransforms.push_back(lightProj * Matrix::rotationAroundZ(180.0f) * lightTrans);

            // Specify what to render an start acquiring
            GLenum buffers[] = { GL_DEPTH_ATTACHMENT };

            //Need to change viewport to the resolution of the shadow texture
            glViewport(0, 0, _cubeTextureMap.getWidth(), _cubeTextureMap.getHeight());

            //Bind frame buffer
            glBindFramebuffer(GL_FRAMEBUFFER, _cubeTextureMap.getCubeDepthFrameBufferContext());

            //Clear depth bufferg8
            glClear(GL_DEPTH_BUFFER_BIT);

            glDrawBuffers(1, buffers);

            for (Model* model : modelList) {
                if (model->getClassType() == ModelClass::ModelType) {
                    _pointShadowShader.runShader(model, light, lightTransforms);
                }
                else if (model->getClassType() == ModelClass::AnimatedModelType) {
                    _pointAnimatedShadowShader.runShader(model, light, lightTransforms);
                }
            }

            //remove framebuffer context
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }

    //Need to change viewport to the resolution of the window size
    glViewport(0, 0, screenPixelWidth, screenPixelHeight);
}