#include "ShaderBase.h"

ShaderBase::ShaderBase() {
    
}

ShaderBase::~ShaderBase() {

}

ShaderBase::ShaderBase(const ShaderBase& shader) {
    *this = shader;
}

Shader* ShaderBase::getShader() {
    return _shader;
}
