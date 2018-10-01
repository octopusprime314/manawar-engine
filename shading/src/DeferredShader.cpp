#include "DeferredShader.h"
#include "ShadowedPointLight.h"
#include "ShadowedDirectionalLight.h"
#include "GLSLShader.h"
#include "HLSLShader.h"
#include "EngineManager.h"

DeferredShader::DeferredShader(std::string shaderName) {

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        _shader = new GLSLShader(shaderName);
    }
    else {
        _shader = new HLSLShader(shaderName);
    }

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
    ViewEventDistributor* viewEventDistributor,
    MRTFrameBuffer& mrtFBO,
    SSAO* ssao,
    EnvironmentMap* environmentMap) {

    //Take the generated texture data and do deferred shading
    //LOAD IN SHADER
    _shader->bind(); //use context for loaded shader

    glBindVertexArray(_vaoContext);

    //Get light view matrix "look at" vector which is located in the third column
    //of the inner rotation matrix at index 2,6,10
    auto viewMatrix = (viewEventDistributor->getView() * lights[0]->getLightMVP().getViewMatrix()).getFlatBuffer();

    Vector4 lightPosition(viewMatrix[2], viewMatrix[6], viewMatrix[10]);
    //lightPosition.display();
    _shader->updateData("light", lightPosition.getFlatBuffer());

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
            auto pos = viewEventDistributor->getView() * light->getPosition();
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

    _shader->updateData("numPointLights", &pointLights);
    _shader->updateData("pointLightColors[0]", lightColorsArray);
    _shader->updateData("pointLightRanges[0]", lightRangesArray);
    _shader->updateData("pointLightPositions[0]", lightPosArray);
    delete[] lightPosArray;  delete[] lightColorsArray; delete[] lightRangesArray;

    //Change of basis from camera view position back to world position
    MVP lightMVP = lights[0]->getLightMVP();
    Matrix cameraToLightSpace = lightMVP.getProjectionMatrix() *
        lightMVP.getViewMatrix() *
        viewEventDistributor->getView().inverse();

    _shader->updateData("lightViewMatrix", cameraToLightSpace.getFlatBuffer());

    //Change of basis from camera view position back to world position
    MVP lightMapMVP = lights[1]->getLightMVP();
    Matrix cameraToLightMapSpace = lightMapMVP.getProjectionMatrix() *
        lightMapMVP.getViewMatrix() *
        viewEventDistributor->getView().inverse();

    _shader->updateData("lightMapViewMatrix", cameraToLightMapSpace.getFlatBuffer());

    //Change of basis from camera view position back to world position
    Matrix viewToModelSpace = viewEventDistributor->getView().inverse();

    _shader->updateData("viewToModelMatrix", viewToModelSpace.getFlatBuffer());

    Matrix projectionToViewSpace = (viewEventDistributor->getProjection()).inverse();
    _shader->updateData("projectionToViewMatrix", projectionToViewSpace.getFlatBuffer());

    _shader->updateData("inverseView", viewEventDistributor->getView().inverse().getFlatBuffer());
    _shader->updateData("inverseProjection", viewEventDistributor->getProjection().inverse().getFlatBuffer());

    auto viewState = viewEventDistributor->getViewState();
    _shader->updateData("views", &viewState);

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

    auto textures = mrtFBO.getTextures();
    

    _shader->updateData("diffuseTexture",     GL_TEXTURE0, &textures[0]);
    _shader->updateData("normalTexture",      GL_TEXTURE1, &textures[1]);
    _shader->updateData("velocityTexture",    GL_TEXTURE2, &textures[2]);
    _shader->updateData("depthTexture",       GL_TEXTURE3, &textures[3]);
    if (directionalShadowTextures.size() > 0) {
        _shader->updateData("cameraDepthTexture", GL_TEXTURE4, directionalShadowTextures[0]->getDepthTexture());
    }
    if (directionalShadowTextures.size() > 1) {
        _shader->updateData("mapDepthTexture", GL_TEXTURE5, directionalShadowTextures[1]->getDepthTexture());
    }
    if (pointShadowTexture != nullptr) {
        _shader->updateData("depthMap", GL_TEXTURE6, pointShadowTexture->getDepthTexture());
    }
    _shader->updateData("skyboxDayTexture",   GL_TEXTURE7, _skyBoxDayTexture);
    _shader->updateData("skyboxNightTexture", GL_TEXTURE8, _skyBoxNightTexture);
    _shader->updateData("ssaoTexture",        GL_TEXTURE9, ssao->getBlur()->getTexture());

    //Draw triangles using the bound buffer vertices at starting index 0 and number of vertices
    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)4);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0); //Unbind texture
    glUseProgram(0);//end using this shader
}
