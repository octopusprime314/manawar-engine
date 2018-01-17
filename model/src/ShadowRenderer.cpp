#include "ShadowRenderer.h"

ShadowRenderer::ShadowRenderer() {
    _staticShadowShader.build("shaders/staticShadowShader");
    _animatedShadowShader.build("shaders/animatedShadowShader");

    //Bone uniforms glUniform mat4 
    _bonesLocation = glGetUniformLocation(_animatedShadowShader.getShaderContext(), "bones");

    _lightShadowView = Matrix::cameraTranslation(0.0, 0.0, 25.0) * Matrix::cameraRotationAroundX(-90.0f);
    _lightPosition = Vector4(0.0, 25.0, 0.0, 1.0);
    _orthographicProjection = Matrix::cameraOrtho(50.0, 50.0, 0.1, 100.0);

}

ShadowRenderer::~ShadowRenderer() {

}

GLuint ShadowRenderer::getDepthTexture() {
    return _shadowFBO.getTextureContext();
}

Matrix ShadowRenderer::getLightShadowView(){
    return _lightShadowView;
}

Matrix ShadowRenderer::getLightShadowProjection(){
    return _orthographicProjection;
}

Vector4 ShadowRenderer::getLightPosition(){
    return _lightPosition;
}


void ShadowRenderer::generateShadowBuffer(std::vector<Model*> modelList) {

    //Bind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, _shadowFBO.getFrameBufferContext());

    //Clear color buffer from frame buffer otherwise framebuffer will contain data from the last draw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // Specify what to render an start acquiring
    GLenum buffers[] = { GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, buffers);


    for (Model* model : modelList) {

        //Load in vbo buffers
        VBO *vbo = model->getVBO();
        MVP* mvp = model->getMVP();
        
        if (model->getClassType() == ModelClass::ModelType) {

            //Use one single shadow shader and replace the vbo buffer from each model
            glUseProgram(_staticShadowShader.getShaderContext()); //use context for loaded shader

            //Bind vertex buff context to current buffer
            glBindBuffer(GL_ARRAY_BUFFER, vbo->getVertexContext());

            //Say that the vertex data is associated with attribute 0 in the context of a shader program
            //Each vertex contains 3 floats per vertex
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

            //Now enable vertex buffer at location 0
            glEnableVertexAttribArray(0);

            //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
            glUniformMatrix4fv(_staticShadowShader.getModelLocation(), 1, GL_TRUE, mvp->getModelBuffer());

            //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
            glUniformMatrix4fv(_staticShadowShader.getViewLocation(), 1, GL_TRUE, _lightShadowView.getFlatBuffer());

            //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
            glUniformMatrix4fv(_staticShadowShader.getProjectionLocation(), 1, GL_TRUE, _orthographicProjection.getFlatBuffer());

            auto textureStrides = model->getTextureStrides();
            unsigned int verticesSize = 0;
            for (auto textureStride : textureStrides) {
                verticesSize += textureStride.second;
            }

            //Draw triangles using the bound buffer vertices at starting index 0 and number of vertices
            glDrawArrays(GL_TRIANGLES, 0, (GLsizei)verticesSize);

            glDisableVertexAttribArray(0); //Disable vertex attribute
            glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind buffer
            glUseProgram(0);//end using this shader
        }
        else {

            AnimatedModel* animationModel = static_cast<AnimatedModel*>(model);

            //Use one single shadow shader and replace the vbo buffer from each model
            glUseProgram(_animatedShadowShader.getShaderContext()); //use context for loaded shader

            //Bind vertex buff context to current buffer
            glBindBuffer(GL_ARRAY_BUFFER, vbo->getVertexContext());

            //Say that the vertex data is associated with attribute 0 in the context of a shader program
            //Each vertex contains 3 floats per vertex
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

            //Now enable vertex buffer at location 0
            glEnableVertexAttribArray(0);

            //Bind bone index buff context to current buffer
            glBindBuffer(GL_ARRAY_BUFFER, animationModel->getIndexContext());

            //First 4 indexes
            //Specify stride to be 8 because the beginning of each index attribute value is 8 bytes away
            //Say that the bone index data is associated with attribute 3 in the context of a shader program
            //Each bone index contains 4 floats per index
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), 0);

            //Now enable bone index buffer at location 1
            glEnableVertexAttribArray(1);

            //Second 4 indexes
            //Specify stride to be 8 because the beginning of each index attribute value is 8 bytes away
            //Specify offset for attribute location of indexes2 to be 4 bytes offset from the beginning location of the buffer
            //Say that the bone index data is associated with attribute 3 in the context of a shader program
            //Each bone index contains 4 floats per index
            glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(4 * sizeof(GL_FLOAT)));

            //Now enable bone index buffer at location 2
            glEnableVertexAttribArray(2);

            //Bind weight buff context to current buffer
            glBindBuffer(GL_ARRAY_BUFFER, animationModel->getWeightContext());

            //First 4 weights
            //Specify stride to be 8 because the beginning of each weight attribute value is 8 bytes away
            //Say that the weight data is associated with attribute 4 in the context of a shader program
            //Each weight contains 4 floats per index
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), 0);

            //Now enable weight buffer at location 3
            glEnableVertexAttribArray(3);

            //Second 4 weights
            //Specify stride to be 8 because the beginning of each weight attribute value is 8 bytes away
            //Specify offset for attribute location of weights2 to be 4 bytes offset from the beginning location of the buffer
            //Say that the weight data is associated with attribute 4 in the context of a shader program
            //Each weight contains 4 floats per index
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(4 * sizeof(GL_FLOAT)));

            //Now enable weight buffer at location 4
            glEnableVertexAttribArray(4);

            //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
            glUniformMatrix4fv(_animatedShadowShader.getModelLocation(), 1, GL_TRUE, mvp->getModelBuffer());

            //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
            glUniformMatrix4fv(_animatedShadowShader.getViewLocation(), 1, GL_TRUE, _lightShadowView.getFlatBuffer());

            //glUniform mat4 projection matrix, GL_TRUE is telling GL we are passing in the matrix as row major
            glUniformMatrix4fv(_animatedShadowShader.getProjectionLocation(), 1, GL_TRUE, _orthographicProjection.getFlatBuffer());

            //Bone uniforms
            auto bones = animationModel->getBones();
            float* bonesArray = new float[16 * bones->size()]; //4x4 times number of bones
            int bonesArrayIndex = 0;
            for (auto bone : *bones) {
                for (int i = 0; i < 16; i++) {
                    float* buff = bone.getFlatBuffer();
                    bonesArray[bonesArrayIndex++] = buff[i];
                }
            }
            glUniformMatrix4fv(_bonesLocation, static_cast<GLsizei>(bones->size()), GL_TRUE, bonesArray);
            delete[] bonesArray;

            auto textureStrides = model->getTextureStrides();
            unsigned int verticesSize = 0;
            for (auto textureStride : textureStrides) {
                verticesSize += textureStride.second;
            }

            //Draw triangles using the bound buffer vertices at starting index 0 and number of vertices
            glDrawArrays(GL_TRIANGLES, 0, (GLsizei)verticesSize);

            glDisableVertexAttribArray(0); //Disable vertex attribute
            glDisableVertexAttribArray(1); //Disable indexes attribute
            glDisableVertexAttribArray(2); //Disable weight attribute
            glDisableVertexAttribArray(3); //Disable indexes2 attribute
            glDisableVertexAttribArray(4); //Disable weight2 attribute
            glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind buffer
            glUseProgram(0);//end using this shader
        }
    }

    //remove framebuffer context
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}