#include "DeferredShader.h"

DeferredShader::DeferredShader(std::string shaderName) : Shader(shaderName) {
    glCheck();

    //Build 2 triangles for screen space quad
    const float length = 1.0f;
    const float depth = 0.0f;
    //2 triangles in screen space
    float triangles[] = { -length, -length, depth,
                         -length, length, depth,
                         length, length, depth,

                         -length, -length, depth,
                         length, length, depth,
                         length, -length, depth };

    glGenBuffers(1, &_quadBufferContext);
    glBindBuffer(GL_ARRAY_BUFFER, _quadBufferContext);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 3, triangles, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //2 texture coordinates in screen space
    float textures[] = { 0.0, 0.0,
                          0.0, 1.0,
                          1.0, 1.0,

                          0.0, 0.0,
                          1.0, 1.0,
                          1.0, 0.0 };

    glGenBuffers(1, &_textureBufferContext);
    glBindBuffer(GL_ARRAY_BUFFER, _textureBufferContext);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 2, textures, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &_vaoContext);
    glBindVertexArray(_vaoContext);

    glBindBuffer(GL_ARRAY_BUFFER, _quadBufferContext);

    //Say that the vertex data is associated with attribute 0 in the context of a shader program
    //Each vertex contains 3 floats per vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //Now enable vertex buffer at location 0
    glEnableVertexAttribArray(0);

    //Bind texture coordinate buff context to current buffer
    glBindBuffer(GL_ARRAY_BUFFER, _textureBufferContext);

    //Say that the texture coordinate data is associated with attribute 2 in the context of a shader program
    //Each texture coordinate contains 2 floats per texture
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    //Now enable texture buffer at location 2
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    //Manually find the two texture locations for loaded shader
    _diffuseTextureLocation = glGetUniformLocation(_shaderContext, "diffuseTexture");
    _normalTextureLocation = glGetUniformLocation(_shaderContext, "normalTexture");
    _positionTextureLocation = glGetUniformLocation(_shaderContext, "positionTexture");
    _cameraDepthTextureLocation = glGetUniformLocation(_shaderContext, "cameraDepthTexture");
    _mapDepthTextureLocation = glGetUniformLocation(_shaderContext, "mapDepthTexture");
    _lightLocation = glGetUniformLocation(_shaderContext, "light");
    _lightViewLocation = glGetUniformLocation(_shaderContext, "lightViewMatrix");
    _lightMapViewLocation = glGetUniformLocation(_shaderContext, "lightMapViewMatrix");
    _viewsLocation = glGetUniformLocation(_shaderContext, "views");
    _pointLightCountLocation = glGetUniformLocation(_shaderContext, "numPointLights");
    _pointLightColorsLocation = glGetUniformLocation(_shaderContext, "pointLightColors");
    _pointLightRangesLocation = glGetUniformLocation(_shaderContext, "pointLightRanges");
    _pointLightPositionsLocation = glGetUniformLocation(_shaderContext, "pointLightPositions");
    _pointLightDepthMapLocation = glGetUniformLocation(_shaderContext, "depthMap");
    _viewToModelSpaceMatrixLocation = glGetUniformLocation(_shaderContext, "viewToModelMatrix");
    _farPlaneLocation = glGetUniformLocation(_shaderContext, "farPlane");

    _inverseViewLocation = glGetUniformLocation(_shaderContext, "inverseView");
    _inverseProjectionLocation = glGetUniformLocation(_shaderContext, "inverseProjection");
    _skyboxDayTextureLocation = glGetUniformLocation(_shaderContext, "skyboxDayTexture");
    _skyboxNightTextureLocation = glGetUniformLocation(_shaderContext, "skyboxNightTexture");
    _ssaoTextureLocation = glGetUniformLocation(_shaderContext, "ssaoTexture");
    _environmentMapTextureLocation = glGetUniformLocation(_shaderContext, "environmentMapTexture");
    _bloomTextureLocation = glGetUniformLocation(_shaderContext, "bloomTexture");

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
    glUniform3f(_lightLocation, viewMatrix[2], viewMatrix[6], viewMatrix[10]);

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
    glUniform1i(_pointLightCountLocation, static_cast<GLsizei>(pointLights));
    glUniform3fv(_pointLightPositionsLocation, static_cast<GLsizei>(pointLights), lightPosArray);
    glUniform3fv(_pointLightColorsLocation, static_cast<GLsizei>(pointLights), lightColorsArray);
    glUniform1fv(_pointLightRangesLocation, static_cast<GLsizei>(pointLights), lightRangesArray);
    delete[] lightPosArray;  delete[] lightColorsArray; delete[] lightRangesArray;

    //Change of basis from camera view position back to world position
    MVP lightMVP = lights[0]->getLightMVP();
    Matrix cameraToLightSpace = lightMVP.getProjectionMatrix() *
        lightMVP.getViewMatrix() *
        viewManager->getView().inverse();

    //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_lightViewLocation, 1, GL_TRUE, cameraToLightSpace.getFlatBuffer());

    //Change of basis from camera view position back to world position
    MVP lightMapMVP = lights[1]->getLightMVP();
    Matrix cameraToLightMapSpace = lightMapMVP.getProjectionMatrix() *
        lightMapMVP.getViewMatrix() *
        viewManager->getView().inverse();

    //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_lightMapViewLocation, 1, GL_TRUE, cameraToLightMapSpace.getFlatBuffer());

    //Change of basis from camera view position back to world position
    Matrix viewToModelSpace = viewManager->getView().inverse();

    //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_viewToModelSpaceMatrixLocation, 1, GL_TRUE, viewToModelSpace.getFlatBuffer());

    //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_inverseViewLocation, 1, GL_TRUE, viewManager->getView().inverse().getFlatBuffer());
    //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_inverseProjectionLocation, 1, GL_TRUE, viewManager->getProjection().inverse().getFlatBuffer());

    glUniform1i(_viewsLocation, static_cast<GLint>(viewManager->getViewState()));

    auto projMatrix = viewManager->getProjection().getFlatBuffer();
    float nearVal = (2.0f*projMatrix[11]) / (2.0f*projMatrix[10] - 2.0f);
    float farVal = ((projMatrix[10] - 1.0f)*nearVal) / (projMatrix[10] + 1.0f);
    glUniform1f(_farPlaneLocation, farVal);

    auto textures = mrtFBO.getTextureContexts();

    //glUniform texture
    glUniform1i(_diffuseTextureLocation, 0);
    glUniform1i(_normalTextureLocation, 1);
    glUniform1i(_positionTextureLocation, 2);
    glUniform1i(_cameraDepthTextureLocation, 3);
    glUniform1i(_mapDepthTextureLocation, 4);
    glUniform1i(_pointLightDepthMapLocation, 5);
    glUniform1i(_skyboxDayTextureLocation, 6);
    glUniform1i(_skyboxNightTextureLocation, 7);
    glUniform1i(_ssaoTextureLocation, 8);
    //glUniform1i(_environmentMapTextureLocation, 9);

    //Diffuse texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);

    //Normal texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    //Position texture
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textures[2]);

    //Depth texture
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, shadowRenderer->getStaticDepthTexture());

    //Depth texture
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, shadowRenderer->getMapDepthTexture());

    //Depth cube texture map for point lights
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, pointShadowMap->getCubeMapTexture());

    //Skybox day and night textures
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _skyBoxDayTexture->getContext());
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _skyBoxNightTexture->getContext());

    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, ssao->getBlur()->getTextureContext());

    /*glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap->getCubeMapTexture());*/

    //Draw triangles using the bound buffer vertices at starting index 0 and number of vertices
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)6);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0); //Unbind texture
    glUseProgram(0);//end using this shader
}
