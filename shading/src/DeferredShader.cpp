#include "DeferredShader.h"

DeferredShader::DeferredShader(std::string shaderName) : Shader(shaderName) {
    glCheck();
    glGenVertexArrays(1, &_vaoContext);

    //Get skybox texture
    TextureBroker* textureManager = TextureBroker::instance();
    //Day sky box
    textureManager->addCubeTexture(TEXTURE_LOCATION + "skybox-day");
    _skyBoxDayTexture = textureManager->getTexture(TEXTURE_LOCATION + "skybox-day");

    //Night sky box
    textureManager->addCubeTexture(TEXTURE_LOCATION + "skybox-night");
    _skyBoxNightTexture = textureManager->getTexture(TEXTURE_LOCATION + "skybox-night");
}

DeferredShader::~DeferredShader() {

}

void DeferredShader::runShader(ShadowRenderer* shadowRenderer,
    std::vector<Light*>& lights,
    ViewManager* viewManager,
    MRTFrameBuffer& mrtFBO,
    PointShadowMap* pointShadowMap,
    SSAO* ssao,
    EnvironmentMap* environmentMap) {
    //Take the generated texture data and do deferred shading
    //LOAD IN SHADER
    glUseProgram(_shaderContext); //use context for loaded shader

    glBindVertexArray(_vaoContext);

    //Get light view matrix "look at" vector which is located in the third column
    //of the inner rotation matrix at index 2,6,10
    auto viewMatrix = lights[0]->getLightMVP().getViewBuffer();
    Vector4 lightPosition(viewMatrix[2], viewMatrix[6], viewMatrix[10]);
    updateUniform("light", lightPosition.getFlatBuffer());

    //Get point light positions
    //TODO add max point light constant
    unsigned int pointLights = 0;
    for (auto& light : lights) {
        if (light->getType() == LightType::POINT) {
            pointLights++;
        }
    }
    float* lightPosArray = new float[3 * pointLights];
    float* lightColorsArray = new float[3 * pointLights];
    float* lightRangesArray = new float[pointLights];
    int lightArrayIndex = 0;
    int lightRangeIndex = 0;
    for (auto& light : lights) {
        //If point light then add to uniforms
        if (light->getType() == LightType::POINT) {
            //Point lights need to remain stationary so move lights with camera space changes
            auto pos = viewManager->getView() * light->getPosition();
            float* posBuff = pos.getFlatBuffer();
            float* colorBuff = light->getColor().getFlatBuffer();
            for (int i = 0; i < 3; i++) {
                lightPosArray[lightArrayIndex] = posBuff[i];
                lightColorsArray[lightArrayIndex] = colorBuff[i];
                lightArrayIndex++;
            }
            lightRangesArray[lightRangeIndex++] = light->getRange();
        }
    }

    updateUniform("numPointLights", &pointLights);
    updateUniform("pointLightColors[0]", lightColorsArray);
    updateUniform("pointLightRanges[0]", lightRangesArray);
    updateUniform("pointLightPositions[0]", lightPosArray);
    delete[] lightPosArray;  delete[] lightColorsArray; delete[] lightRangesArray;

    //Change of basis from camera view position back to world position
    MVP lightMVP = lights[0]->getLightMVP();
    Matrix cameraToLightSpace = lightMVP.getProjectionMatrix() *
        lightMVP.getViewMatrix() *
        viewManager->getView().inverse();

    updateUniform("lightViewMatrix", cameraToLightSpace.getFlatBuffer());

    //Change of basis from camera view position back to world position
    MVP lightMapMVP = lights[1]->getLightMVP();
    Matrix cameraToLightMapSpace = lightMapMVP.getProjectionMatrix() *
        lightMapMVP.getViewMatrix() *
        viewManager->getView().inverse();

    updateUniform("lightMapViewMatrix", cameraToLightMapSpace.getFlatBuffer());

    //Change of basis from camera view position back to world position
    Matrix viewToModelSpace = viewManager->getView().inverse();

    updateUniform("viewToModelMatrix", viewToModelSpace.getFlatBuffer());

    updateUniform("inverseView", viewManager->getView().inverse().getFlatBuffer());
    updateUniform("inverseProjection", viewManager->getProjection().inverse().getFlatBuffer());

    auto viewState = viewManager->getViewState();
    updateUniform("views", &viewState);

    auto projMatrix = viewManager->getProjection().getFlatBuffer();
    float nearVal = (2.0f*projMatrix[11]) / (2.0f*projMatrix[10] - 2.0f);
    float farVal = ((projMatrix[10] - 1.0f)*nearVal) / (projMatrix[10] + 1.0f);
    updateUniform("farPlane", &farVal);

    auto textures = mrtFBO.getTextureContexts();

    updateUniform("diffuseTexture",     GL_TEXTURE0, textures[0],                             GL_TEXTURE_2D);
    updateUniform("normalTexture",      GL_TEXTURE1, textures[1],                             GL_TEXTURE_2D);
    updateUniform("positionTexture",    GL_TEXTURE2, textures[2],                             GL_TEXTURE_2D);
    updateUniform("cameraDepthTexture", GL_TEXTURE3, shadowRenderer->getStaticDepthTexture(), GL_TEXTURE_2D);
    updateUniform("mapDepthTexture",    GL_TEXTURE4, shadowRenderer->getMapDepthTexture(),    GL_TEXTURE_2D);
    updateUniform("depthMap",           GL_TEXTURE5, pointShadowMap->getCubeMapTexture(),     GL_TEXTURE_CUBE_MAP);
    updateUniform("skyboxDayTexture",   GL_TEXTURE6, _skyBoxDayTexture->getContext(),         GL_TEXTURE_CUBE_MAP);
    updateUniform("skyboxNightTexture", GL_TEXTURE7, _skyBoxNightTexture->getContext(),       GL_TEXTURE_CUBE_MAP);
    updateUniform("ssaoTexture",        GL_TEXTURE8, ssao->getBlur()->getTextureContext(),    GL_TEXTURE_2D);

    //Draw triangles using the bound buffer vertices at starting index 0 and number of vertices
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)4);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0); //Unbind texture
    glUseProgram(0);//end using this shader
}
