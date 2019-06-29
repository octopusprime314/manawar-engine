#include "ForwardShader.h"
#include "MVP.h"
#include "VAO.h"
#include "Model.h"
#include "ViewEventDistributor.h"
#include "PointShadow.h"
#include "Entity.h"
#include "ShadowedPointLight.h"
#include "ShadowedDirectionalLight.h"
#include "GLSLShader.h"
#include "HLSLShader.h"
#include "EngineManager.h"

ForwardShader::ForwardShader(std::string vertexShaderName, std::string fragmentShaderName) {
    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        _shader = new GLSLShader(vertexShaderName, fragmentShaderName);
    }
    else {
        std::vector<DXGI_FORMAT>* formats = new std::vector<DXGI_FORMAT>();
        formats->push_back(DXGI_FORMAT_R8G8B8A8_UNORM);
        formats->push_back(DXGI_FORMAT_D32_FLOAT);
        _shader = new HLSLShader(vertexShaderName, "", formats);
    }
}

ForwardShader::~ForwardShader() {

}

void ForwardShader::runShader(Entity* entity, ViewEventDistributor* viewEventDistributor,
    std::vector<Light*>& lights) {

    auto model = entity->getModel();
    if (model->getClassType() != ModelClass::AnimatedModelType)
    {
        //LOAD IN SHADER
        _shader->bind(); //use context for loaded shader

        //LOAD IN VAO
        std::vector<VAO*>* vao = entity->getFrustumVAO();
        for (auto vaoInstance : *vao) {
            _shader->bindAttributes(vaoInstance);

            MVP* mvp = entity->getMVP();
            //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
            _shader->updateData("model", mvp->getModelBuffer());

            //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
            _shader->updateData("view", mvp->getViewBuffer());

            //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
            _shader->updateData("projection", mvp->getProjectionBuffer());

            //glUniform mat4 normal matrix, GL_TRUE is telling GL we are passing in the matrix as row major
            _shader->updateData("normalMatrix", mvp->getNormalBuffer());

            //Get light view matrix "look at" vector which is located in the third column
            //of the inner rotation matrix at index 2,6,10
            auto viewMatrix = lights[0]->getLightMVP().getViewMatrix().getFlatBuffer();
            Vector4 lightPosition(viewMatrix[2], viewMatrix[6], viewMatrix[10]);
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
            if (lights.size() > 0 && lights[0] != nullptr) {
                MVP lightMVP = lights[0]->getLightMVP();
                Matrix cameraToLightSpace = lightMVP.getProjectionMatrix() *
                    lightMVP.getViewMatrix() *
                    viewEventDistributor->getView().inverse();

                _shader->updateData("lightViewMatrix", cameraToLightSpace.getFlatBuffer());
            }

            if (lights.size() > 1 && lights[1] != nullptr) {
                //Change of basis from camera view position back to world position
                MVP lightMapMVP = lights[1]->getLightMVP();
                Matrix cameraToLightMapSpace = lightMapMVP.getProjectionMatrix() *
                    lightMapMVP.getViewMatrix() *
                    viewEventDistributor->getView().inverse();

                _shader->updateData("lightMapViewMatrix", cameraToLightMapSpace.getFlatBuffer());
            }

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

                    //Draw triangles using the bound buffer vertices at starting index 0 and number of vertices
                    _shader->draw(strideLocation, 1, (GLsizei)textureStride.second);
                }
                strideLocation += textureStride.second;
            }

            _shader->unbindAttributes();
        }
        _shader->unbind();
    }
}
