#include "ForwardShader.h"
#include "MVP.h"
#include "VAO.h"
#include "Model.h"
#include "ViewManager.h"
#include "PointShadow.h"
#include "Entity.h"
#include "ShadowedPointLight.h"
#include "ShadowedDirectionalLight.h"

ForwardShader::ForwardShader(std::string vertexShaderName, std::string fragmentShaderName)
    : Shader(vertexShaderName, fragmentShaderName) {
}

ForwardShader::~ForwardShader() {

}

void ForwardShader::runShader(Entity* entity, ViewManager* viewManager,
    std::vector<Light*>& lights) {

    //LOAD IN SHADER
    glUseProgram(_shaderContext); //use context for loaded shader

    auto model = entity->getModel();
    //LOAD IN VAO
    VAO* vao = model->getVAO();
    glBindVertexArray(vao->getVAOContext());

    MVP* mvp = entity->getMVP();
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
            if (directionalShadowTextures.size() > 0) {
                updateUniform("cameraDepthTexture", GL_TEXTURE1, directionalShadowTextures[0]->getDepthTexture());
            }
            if (directionalShadowTextures.size() > 1) {
                updateUniform("mapDepthTexture", GL_TEXTURE2, directionalShadowTextures[1]->getDepthTexture());
            }
            updateUniform("depthMap",           GL_TEXTURE3, pointShadowTexture->getDepthTexture());

            //Draw triangles using the bound buffer vertices at starting index 0 and number of triangles
            glDrawArrays(GL_TRIANGLES, strideLocation, (GLsizei)textureStride.second);
        }
        strideLocation += textureStride.second;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    glUseProgram(0);//end using this shader

}
