#include "DeferredShader.h"
#include "ShadowedPointLight.h"
#include "ShadowedDirectionalLight.h"

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

void DeferredShader::runShader(std::vector<Light*>& lights,
    ViewManager* viewManager,
    MRTFrameBuffer& mrtFBO,
    SSAO* ssao,
    EnvironmentMap* environmentMap) {

    //Take the generated texture data and do deferred shading
    //LOAD IN SHADER
    glUseProgram(_shaderContext); //use context for loaded shader

    glBindVertexArray(_vaoContext);

    //Get light view matrix "look at" vector which is located in the third column
    //of the inner rotation matrix at index 2,6,10
    auto viewMatrix = (viewManager->getView() * lights[0]->getLightMVP().getViewMatrix()).getFlatBuffer();

    Vector4 lightPosition(viewMatrix[2], viewMatrix[6], viewMatrix[10]);
    //lightPosition.display();
    updateUniform("light", lightPosition.getFlatBuffer());

    //Get point light positions
    //TODO add max point light constant
    unsigned int pointLights = 0;
    for (auto& light : lights) {
        if (light->getType() == LightType::POINT || 
            light->getType() == LightType::SHADOWED_POINT) {
            pointLights++;
        }
    }
    //Constant max of 20 lights in shader
    float* lightPosArray = new float[3 * 20];
    float* lightColorsArray = new float[3 * 20];
    float* lightRangesArray = new float[20];
    int lightArrayIndex = 0;
    int lightRangeIndex = 0;
    for (auto& light : lights) {
        //If point light then add to uniforms
        if (light->getType() == LightType::POINT ||
            light->getType() == LightType::SHADOWED_POINT) {
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

    Matrix projectionToViewSpace = (viewManager->getProjection()).inverse();
    updateUniform("projectionToViewMatrix", projectionToViewSpace.getFlatBuffer());

    updateUniform("inverseView", viewManager->getView().inverse().getFlatBuffer());
    updateUniform("inverseProjection", viewManager->getProjection().inverse().getFlatBuffer());

    auto viewState = viewManager->getViewState();
    updateUniform("views", &viewState);

    ShadowedPointLight* pointShadowTexture = nullptr;
    std::vector<ShadowedDirectionalLight*> directionalShadowTextures;
    for (auto light : lights) {
        if (light->getType() == LightType::SHADOWED_POINT) {
            pointShadowTexture = static_cast<ShadowedPointLight*>(light);
            break;
        }
        else if (light->getType() == LightType::SHADOWED_DIRECTIONAL) {
            directionalShadowTextures.push_back(static_cast<ShadowedDirectionalLight*>(light));
        }
    }

    auto textures = mrtFBO.getTextureContexts();

    updateUniform("diffuseTexture",     GL_TEXTURE0, textures[0]);
    updateUniform("normalTexture",      GL_TEXTURE1, textures[1]);
    updateUniform("velocityTexture",    GL_TEXTURE2, textures[2]);
    updateUniform("depthTexture",       GL_TEXTURE3, textures[3]);
    if (directionalShadowTextures.size() > 0) {
        updateUniform("cameraDepthTexture", GL_TEXTURE4, directionalShadowTextures[0]->getDepthTexture());
    }
    if (directionalShadowTextures.size() > 1) {
        updateUniform("mapDepthTexture", GL_TEXTURE5, directionalShadowTextures[1]->getDepthTexture());
    }
    if (pointShadowTexture != nullptr) {
        updateUniform("depthMap", GL_TEXTURE6, pointShadowTexture->getDepthTexture());
    }
    updateUniform("skyboxDayTexture",   GL_TEXTURE7, _skyBoxDayTexture->getContext());
    updateUniform("skyboxNightTexture", GL_TEXTURE8, _skyBoxNightTexture->getContext());
    updateUniform("ssaoTexture",        GL_TEXTURE9, ssao->getBlur()->getTextureContext());

    //Draw triangles using the bound buffer vertices at starting index 0 and number of vertices
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)4);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0); //Unbind texture
    glUseProgram(0);//end using this shader
}
