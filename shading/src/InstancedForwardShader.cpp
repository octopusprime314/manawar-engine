#include "InstancedForwardShader.h"
#include "ForwardShader.h"
#include "MVP.h"
#include "VAO.h"
#include "Model.h"
#include "ViewEventDistributor.h"
#include "PointShadow.h"
#include "Entity.h"
#include "ShadowedPointLight.h"
#include "ShadowedDirectionalLight.h"

InstancedForwardShader::InstancedForwardShader(std::string shaderName) : ForwardShader(shaderName, "forwardShader") {

}
InstancedForwardShader::~InstancedForwardShader() {

}

void InstancedForwardShader::runShader(Entity* entity, ViewEventDistributor* viewEventDistributor, 
    std::vector<Light*>& lights) {

    //LOAD IN SHADER
    _shader->bind(); //use context for loaded shader

    //LOAD IN VAO
    auto model = entity->getModel();
    std::vector<VAO*>* vao = model->getVAO();

    for (auto vaoInstance : *vao) {
        glBindVertexArray(vaoInstance->getVAOContext());

        MVP* mvp = entity->getMVP();
        //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        _shader->updateData("model", mvp->getModelBuffer());

        //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        _shader->updateData("view", mvp->getViewBuffer());

        //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        _shader->updateData("projection", mvp->getProjectionBuffer());

        //glUniform mat4 normal matrix, GL_TRUE is telling GL we are passing in the matrix as row major
        _shader->updateData("normal", mvp->getNormalBuffer());

        //instance array offsets
        _shader->updateData("offsets[0]", model->getInstanceOffsets());

        _shader->updateData("lightDirection", lights[0]->getLightDirection().getFlatBuffer());

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

        auto textureStrides = vaoInstance->getTextureStrides();
        unsigned int strideLocation = 0;
        for (auto textureStride : textureStrides) {

            //Do not support layered textures or animated models with transparency for now
            //If triangle's textures supports transparency then DO DRAW
            //Only transparent objects are rendered here
            if (textureStride.first.substr(0, 7) != "Layered" &&
                model->getClassType() != ModelClass::AnimatedModelType &&
                model->getTexture(textureStride.first)->getTransparency()) {

                _shader->updateData("textureMap", GL_TEXTURE0, model->getTexture(textureStride.first));
                if (directionalShadowTextures.size() > 0) {
                    _shader->updateData("cameraDepthTexture", GL_TEXTURE1, directionalShadowTextures[0]->getDepthTexture());
                }
                if (directionalShadowTextures.size() > 1) {
                    _shader->updateData("mapDepthTexture", GL_TEXTURE2, directionalShadowTextures[1]->getDepthTexture());
                }
                if (pointShadowTexture != nullptr) {
                    _shader->updateData("depthMap", GL_TEXTURE3, pointShadowTexture->getDepthTexture());
                }

                //Draw triangles using the bound buffer vertices at starting index 0 and number of triangles
                glDrawArraysInstanced(GL_TRIANGLES, strideLocation, (GLsizei)textureStride.second, 110);
            }
            strideLocation += textureStride.second;
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    }
    glUseProgram(0);//end using this shader
}
