#include "DeferredRenderer.h"
#include "Model.h"

DeferredRenderer::DeferredRenderer() : _mrtFBO(2){
    
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
}

DeferredRenderer::~DeferredRenderer() {

}

void DeferredRenderer::build(std::string shaderName) {

    //compile the deferred shader that will use textures rendered by the mrt
    _deferredShader.build("shaders/deferredShader");

    //Manually find the two texture locations for loaded shader
    _diffuseTextureLocation = glGetUniformLocation(_deferredShader.getShaderContext(), "diffuseTexture");
    _normalTextureLocation = glGetUniformLocation(_deferredShader.getShaderContext(), "normalTexture");
    _lightLocation = glGetUniformLocation(_deferredShader.getShaderContext(), "light");
}

void DeferredRenderer::runShader(std::vector<Light*>& lights) {
    //Take the generated texture data and do deferred shading
    _deferredRender(lights);
}

void DeferredRenderer::_deferredRender(std::vector<Light*>& lights) {

	//LOAD IN SHADER
    glUseProgram(_deferredShader.getShaderContext()); //use context for loaded shader

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

    //Compute directional light on cpu side
    Vector4 transformedLight = lights.back()->getNormalMatrix() * lights.back()->getLightDirection();
    float* buff = transformedLight.getFlatBuffer();
    glUniform3f(_lightLocation, -buff[0], -buff[1], -buff[2]);

    auto textures = _mrtFBO.getTextureContexts();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    //glUniform texture 
    glUniform1iARB(_diffuseTextureLocation, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    //glUniform texture 
    glUniform1iARB(_normalTextureLocation, 1);

    //Draw triangles using the bound buffer vertices at starting index 0 and number of vertices
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)6);

    glDisableVertexAttribArray(0); //Disable vertex attribute
    glDisableVertexAttribArray(1); //Disable texture attribute
    glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind buffer
    glUseProgram(0);//end using this shader

}

void DeferredRenderer::bind() {
    //Bind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, _mrtFBO.getFrameBufferContext());

    //Clear color buffer from frame buffer otherwise framebuffer will contain data from the last draw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // Specify what to render an start acquiring
    GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, buffers);
}
void DeferredRenderer::unbind() {
    //unbind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}