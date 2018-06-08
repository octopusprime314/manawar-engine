#include "ComputeShader.h"
#include "Blur.h"
#include "SSAO.h"


ComputeShader::ComputeShader() : Shader("computeShader"){

	_SSAOTextureLocation = glGetUniformLocation(_shaderContext, "ssaoTexture");
	_blurredSSAOTextureLocation = glGetUniformLocation(_shaderContext, "blurredSSAOTexture");
}

ComputeShader::~ComputeShader() {

}

void ComputeShader::runShader(Blur* blur, SSAO* ssao) {

	glUseProgram(_shaderContext);

	//Bind depth textures
	glUniform1i(_SSAOTextureLocation, 0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, ssao->getSSAOTexture());

	//Bind color textures
	glUniform1i(_blurredSSAOTextureLocation, 1);
	glBindImageTexture(1, blur->getBlurTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8);

	//Dispatch the shader
	glDispatchCompute(static_cast<GLuint>(ceilf(static_cast<float>(screenPixelWidth) / 16.0f)),
        static_cast<GLuint>(ceilf(static_cast<float>(screenPixelHeight) / 16.0f)), 1);

	glUseProgram(0);
}
