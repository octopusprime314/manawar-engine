#include "ShadowAnimatedPointShader.h"

ShadowAnimatedPointShader::ShadowAnimatedPointShader(std::string shaderName) : ShadowPointShader(shaderName) {

    _bonesLocation = glGetUniformLocation(_shaderContext, "bones");
}

ShadowAnimatedPointShader::~ShadowAnimatedPointShader() {

}

void ShadowAnimatedPointShader::runShader(Model* model, Light* light, std::vector<Matrix> lightTransforms) {

    AnimatedModel* animationModel = static_cast<AnimatedModel*>(model);

    //Load in vbo buffers
    VAO* vao = animationModel->getVAO();
    MVP* modelMVP = animationModel->getMVP();
    MVP lightMVP = light->getLightMVP();

    //Use one single shadow shader and replace the vbo buffer from each model
    glUseProgram(_shaderContext); //use context for loaded shader

    glBindVertexArray(vao->getVAOShadowContext());

    //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_modelLocation, 1, GL_TRUE, modelMVP->getModelBuffer());

    //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    glUniformMatrix4fv(_viewLocation, 1, GL_TRUE, modelMVP->getViewBuffer());

    float* lightCubeTransforms = new float[6 * 16];
    int index = 0;
    for (Matrix lightTransform : lightTransforms) {
        float* mat = lightTransform.getFlatBuffer();
        for (int i = 0; i < 16; ++i) {
            lightCubeTransforms[index++] = mat[i];
        }
    }
    //glUniform mat4 light cube map transforms, GL_TRUE is telling GL we are passing in the matrix as row major
    //6 faces and each transform is 16 floats in a 4x4 matrix
    glUniformMatrix4fv(_lightCubeTransformsLocation, 6, GL_TRUE, lightCubeTransforms);
    delete[] lightCubeTransforms;

    //Set light position for point light
    auto lightPos = light->getPosition();
    glUniform3f(_lightPosLocation, lightPos.getx(), lightPos.gety(), lightPos.getz());

    //Set far plane for depth scaling
    //Quick trick to get far value out of projection matrix
    auto projMatrix = lightMVP.getProjectionBuffer();
    float nearVal = (2.0f*projMatrix[11]) / (2.0f*projMatrix[10] - 2.0f);
    float farVal = ((projMatrix[10] - 1.0f)*nearVal) / (projMatrix[10] + 1.0f);
    glUniform1f(_farPlaneLocation, farVal);

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

    auto textureStrides = animationModel->getTextureStrides();
    unsigned int verticesSize = 0;
    for (auto textureStride : textureStrides) {
        verticesSize += textureStride.second;
    }

    //Draw triangles using the bound buffer vertices at starting index 0 and number of vertices
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)verticesSize);

    glBindVertexArray(0);
    glUseProgram(0);//end using this shader
}
