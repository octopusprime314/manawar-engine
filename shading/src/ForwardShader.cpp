#include "ForwardShader.h"
#include "MVP.h"
#include "VAO.h"
#include "Model.h"
#include "ViewManager.h"
#include "ShadowRenderer.h"
#include "PointShadowMap.h"

ForwardShader::ForwardShader(std::string vertexShaderName, std::string fragmentShaderName)
    : Shader(vertexShaderName, fragmentShaderName) {
}

ForwardShader::~ForwardShader() {

}

void ForwardShader::runShader(Model* model, ViewManager* viewManager, ShadowRenderer* shadowRenderer,
    std::vector<Light*>& lights, PointShadowMap* pointShadowMap) {

    //LOAD IN SHADER
    glUseProgram(_shaderContext); //use context for loaded shader

    //LOAD IN VAO
    VAO* vao = model->getVAO();
    glBindVertexArray(vao->getVAOContext());

    MVP* mvp = model->getMVP();
    //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    updateUniform("model", mvp->getModelBuffer());

    //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    updateUniform("view", mvp->getViewBuffer());

    //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    updateUniform("projection", mvp->getProjectionBuffer());

    //glUniform mat4 normal matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    updateUniform("normal", mvp->getNormalBuffer());

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

    auto projMatrix = viewManager->getProjection().getFlatBuffer();
    float nearVal = (2.0f*projMatrix[11]) / (2.0f*projMatrix[10] - 2.0f);
    float farVal = ((projMatrix[10] - 1.0f)*nearVal) / (projMatrix[10] + 1.0f);
    updateUniform("farPlane", &farVal);

    auto textureStrides = model->getTextureStrides();
    unsigned int strideLocation = 0;
    for (auto textureStride : textureStrides) {

        //Do not support layered textures or animated models with transparency for now
        //If triangle's textures supports transparency then DO DRAW
        //Only transparent objects are rendered here
        if (textureStride.first.substr(0, 7) != "Layered" && 
            model->getClassType() != ModelClass::AnimatedModelType &&
            model->getTexture(textureStride.first)->getTransparency()) {

            updateUniform("textureMap",         GL_TEXTURE0, model->getTexture(textureStride.first)->getContext());
            updateUniform("cameraDepthTexture", GL_TEXTURE1, shadowRenderer->getAnimatedDepthTexture());
            updateUniform("mapDepthTexture",    GL_TEXTURE2, shadowRenderer->getMapDepthTexture());
            updateUniform("depthMap",           GL_TEXTURE3, pointShadowMap->getCubeMapTexture());

            //Draw triangles using the bound buffer vertices at starting index 0 and number of triangles
            glDrawArrays(GL_TRIANGLES, strideLocation, (GLsizei)textureStride.second);
        }
        strideLocation += textureStride.second;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    glUseProgram(0);//end using this shader

}
