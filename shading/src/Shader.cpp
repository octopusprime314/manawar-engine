#include "Shader.h"
#include "Entity.h"
#include "Light.h"
#include <iostream>
#include <fstream>
#include "EngineManager.h"
#include "DXLayer.h"

// You can hit this in a debugger.
// Set to 'true' to printf every shader that is linked or compiled.
static volatile bool g_VerboseShaders = false;

Shader::Shader() {
    
}

Shader::~Shader() {

}

Shader::Shader(const Shader& shader) {
    *this = shader;
}

bool Shader::_fileExists(const std::string& name) {
    std::ifstream f(name.c_str());
    return        f.good();
}
