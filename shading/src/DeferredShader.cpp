#include "DeferredShader.h"

DeferredShader::DeferredShader(std::string shaderName) : Shader(shaderName) {

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
}

DeferredShader::~DeferredShader() {

}

void DeferredShader::runShader(ShadowRenderer* shadowRenderer, 
							   std::vector<Light*>& lights, 
							   ViewManager* viewManager,
							   MRTFrameBuffer& mrtFBO) {

    //Take the generated texture data and do deferred shading
    //LOAD IN SHADER
    glUseProgram(_shaderContext); //use context for loaded shader

                                                        //LOAD IN VBO BUFFERS 
                                                        //Bind vertex buff context to current buffer
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

    //Get light position
    Vector4 transformedLight = lights[0]->getPosition();
    float* buff = transformedLight.getFlatBuffer();
    glUniform3f(_lightLocation, buff[0], buff[1], buff[2]);

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
            auto pos = light->getPosition();
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
    delete [] lightPosArray;  delete [] lightColorsArray; delete[] lightRangesArray;

    //Change of basis from camera view position back to world position
    MVP lightMVP = lights[0]->getMVP();
    Matrix cameraToLightSpace = lightMVP.getProjectionMatrix() * 
        lightMVP.getViewMatrix() * 
        viewManager->getView().inverse();

    //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_lightViewLocation, 1, GL_TRUE, cameraToLightSpace.getFlatBuffer());

    //Change of basis from camera view position back to world position
    MVP lightMapMVP = lights[1]->getMVP();
    Matrix cameraToLightMapSpace = lightMapMVP.getProjectionMatrix() * 
        lightMapMVP.getViewMatrix() * 
        viewManager->getView().inverse();

    //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_lightMapViewLocation, 1, GL_TRUE, cameraToLightMapSpace.getFlatBuffer());

    glUniform1i(_viewsLocation, static_cast<GLint>(viewManager->getViewState()));

    auto textures = mrtFBO.getTextureContexts();

    //Diffuse texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    //glUniform texture 
    glUniform1i(_diffuseTextureLocation, 0);

    //Normal texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    //glUniform texture 
    glUniform1i(_normalTextureLocation, 1);

    //Position texture
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    //glUniform texture 
    glUniform1i(_positionTextureLocation, 2);

    //Depth texture
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, shadowRenderer->getAnimatedDepthTexture());
    //glUniform texture 
    glUniform1i(_cameraDepthTextureLocation, 3);

    //Depth texture
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, shadowRenderer->getMapDepthTexture());
    //glUniform texture 
    glUniform1i(_mapDepthTextureLocation, 4);

    //Draw triangles using the bound buffer vertices at starting index 0 and number of vertices
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)6);

    glDisableVertexAttribArray(0); //Disable vertex attribute
    glDisableVertexAttribArray(1); //Disable texture attribute
    glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind buffer
    glUseProgram(0);//end using this shader
}