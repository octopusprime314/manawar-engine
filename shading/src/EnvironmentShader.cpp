#include "EnvironmentShader.h"
#include "Model.h"

EnvironmentShader::EnvironmentShader(std::string shaderName) : Shader(shaderName) {

}

EnvironmentShader::~EnvironmentShader() {

}

void EnvironmentShader::runShader(Model* model, std::vector<Matrix> viewTransforms) {

    //LOAD IN SHADER
    glUseProgram(_shaderContext); //use context for loaded shader

    VAO* vao = model->getVAO();
    glBindVertexArray(vao->getVAOContext());

    MVP* mvp = model->getMVP();

    //glUniform mat4 combined model and world matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    updateUniform("model", mvp->getModelBuffer());

    //glUniform mat4 view matrix, GL_TRUE is telling GL we are passing in the matrix as row major
    updateUniform("view", mvp->getViewBuffer());

    float* lightCubeTransforms = new float[6 * 16];
    int index = 0;
    for (Matrix lightTransform : viewTransforms) {
        float* mat = lightTransform.getFlatBuffer();
        for (int i = 0; i < 16; ++i) {
            lightCubeTransforms[index++] = mat[i];
        }
    }
    //glUniform mat4 light cube map transforms, GL_TRUE is telling GL we are passing in the matrix as row major
    //6 faces and each transform is 16 floats in a 4x4 matrix
    updateUniform("viewMatrices[0]", lightCubeTransforms);
    delete[] lightCubeTransforms;

    auto textureStrides = model->getTextureStrides();
    unsigned int strideLocation = 0;
    for (auto textureStride : textureStrides) {

        //If the texture has layered encoded into the string then it is indeed layered
        if (textureStride.first.substr(0, 7) == "Layered") {

            LayeredTexture* layeredTexture = model->getLayeredTexture(textureStride.first);
            auto textures = layeredTexture->getTextures();

            //We have a layered texture
            int isLayered = 1;
            updateUniform("isLayeredTexture", &isLayered);

            if (textures.size() > 4) {
                updateUniform("tex0",      GL_TEXTURE1, textures[0]->getContext(), GL_TEXTURE_2D);
                updateUniform("tex1",      GL_TEXTURE2, textures[1]->getContext(), GL_TEXTURE_2D);
                updateUniform("tex2",      GL_TEXTURE3, textures[2]->getContext(), GL_TEXTURE_2D);
                updateUniform("tex3",      GL_TEXTURE4, textures[3]->getContext(), GL_TEXTURE_2D);
                updateUniform("alphatex0", GL_TEXTURE5, textures[7]->getContext(), GL_TEXTURE_2D);
            }
            else {
                updateUniform("tex0",      GL_TEXTURE1, textures[0]->getContext(), GL_TEXTURE_2D);
                updateUniform("tex1",      GL_TEXTURE2, textures[1]->getContext(), GL_TEXTURE_2D);
                updateUniform("tex2",      GL_TEXTURE3, textures[2]->getContext(), GL_TEXTURE_2D);
                updateUniform("alphatex0", GL_TEXTURE5, textures[3]->getContext(), GL_TEXTURE_2D);
            }
            glDrawArrays(GL_TRIANGLES, strideLocation, (GLsizei)textureStride.second);
            strideLocation += textureStride.second;
        }
        else {
            //If triangle's textures supports transparency then do NOT draw
            //Transparent objects will be rendered after the deferred lighting pass
            if (!model->getTexture(textureStride.first)->getTransparency()) {
                //Not layered texture
                int isLayered = 0;
                updateUniform("isLayeredTexture", &isLayered);

                updateUniform("textureMap", GL_TEXTURE0, model->getTexture(textureStride.first)->getContext(), GL_TEXTURE_2D);

                //Draw triangles using the bound buffer vertices at starting index 0 and number of triangles
                glDrawArrays(GL_TRIANGLES, strideLocation, (GLsizei)textureStride.second);
            }
            strideLocation += textureStride.second;
        }
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture
    glUseProgram(0);//end using this shader
}
