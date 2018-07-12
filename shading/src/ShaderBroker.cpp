#include "ShaderBroker.h"
#include "Dirent.h"
#include "StaticShader.h"
#include "AnimationShader.h"
#include "EffectShader.h"
#include "ShadowStaticShader.h"
#include "SSAOShader.h"
#include "ShadowAnimatedPointShader.h"
#include "ShadowAnimatedShader.h"
#include "ShadowPointShader.h"
#include "MergeShader.h"
#include "ForwardShader.h"
#include "DeferredShader.h"
#include "FontShader.h"
#include "ComputeShader.h"
#include <algorithm>
#include <cctype>
//#include "Logger.h"
ShaderBroker* ShaderBroker::_broker = nullptr;

ShaderBroker* ShaderBroker::instance() { //Only initializes the static pointer once
    if (_broker == nullptr) {
        _broker = new ShaderBroker();
    }
    return _broker;
}
ShaderBroker::ShaderBroker() {

}
ShaderBroker::~ShaderBroker() {

}

//helper function to capitalize everything
std::string ShaderBroker::_strToUpper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::toupper(c); } // correct
    );
    return s;
}

void ShaderBroker::compileShaders() {

    _gatherShaderNames();
}

Shader* ShaderBroker::getShader(std::string shaderName) {
    std::string upperCaseMapName = _strToUpper(shaderName);
    return _shaders[upperCaseMapName];
}

void ShaderBroker::recompileShader(std::string shaderName) {

    std::string upperCaseMapName = _strToUpper(shaderName);

    if (_shaders.find(upperCaseMapName) != _shaders.end()) {

        auto shader = new Shader(upperCaseMapName);
        //Release previous shader
        glDeleteShader(_shaders[upperCaseMapName]->getShaderContext());
        _shaders[upperCaseMapName]->updateShader(shader);
    }
    else {
        std::cout << "Shader doesn't exist so we can't recompile it!" << std::endl;
    }
}


void ShaderBroker::_gatherShaderNames()
{
    bool isFile = false;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(SHADERS_LOCATION.c_str())) != nullptr)
    {
        //Logger::WriteLog("Files to be processed: \n");

        while ((ent = readdir(dir)) != nullptr) {
            if (*ent->d_name) {
                std::string fileName = std::string(ent->d_name);
           
                if (!fileName.empty() && 
                    fileName != "." && 
                    fileName != ".." && 
                    fileName.find(".") != std::string::npos &&
                    fileName.find(".ini") == std::string::npos) {
                    
                    //Logger::WriteLog(ent->d_name, "\n");

                    std::string mapName = fileName.substr(0, fileName.find("."));
                    std::string upperCaseMapName = _strToUpper(mapName);
                    if (mapName == "staticShader") {
                        _shaders[upperCaseMapName] = new StaticShader(mapName);
                    }
                    else if (mapName == "animatedShader") {
                        _shaders[upperCaseMapName] = new AnimationShader(mapName);
                    }
                    else if (mapName == "animatedShadowShader") {
                        _shaders[upperCaseMapName] = new ShadowAnimatedShader(mapName);
                    }
                    else if (mapName == "waterShader" || mapName == "fireShader") {
                        _shaders[upperCaseMapName] = new EffectShader(mapName);
                    }
                    else if (mapName == "staticShadowShader") {
                        _shaders[upperCaseMapName] = new ShadowStaticShader(mapName);
                    }
                    else if (mapName == "ssaoShader") {
                        _shaders[upperCaseMapName] = new SSAOShader();
                    }
                    else if (mapName == "pointShadowShader") {
                        _shaders[upperCaseMapName] = new ShadowPointShader(mapName);
                    }
                    else if (mapName == "pointAnimatedShadowShader") {
                        _shaders[upperCaseMapName] = new ShadowAnimatedPointShader(mapName);
                    }
                    else if (mapName == "mergeShader") {
                        _shaders[upperCaseMapName] = new MergeShader();
                    }
                    else if (mapName == "forwardShader") {
                        _shaders[upperCaseMapName] = new ForwardShader("forwardShader");
                    }
                    else if (mapName == "deferredShader") {
                        _shaders[upperCaseMapName] = new DeferredShader(mapName);
                    }
                    else if (mapName == "add" || 
                        mapName == "blurHorizontalShaderRGB" || 
                        mapName == "blurShader" || 
                        mapName == "blurVerticalShaderRGB" || 
                        mapName == "downsample" || 
                        mapName == "downsampleRGB" ||
                        mapName == "highLuminanceFilter" ||
                        mapName == "motionBlur" ||
                        mapName == "upsample" ||
                        mapName == "upsampleRGB") {
                        _shaders[upperCaseMapName] = new ComputeShader(mapName);
                    }
                    else if (mapName == "fontShader") {
                        _shaders[upperCaseMapName] = new FontShader(mapName);
                    }
                    else if (mapName == "debugShader") {
                        _shaders[upperCaseMapName] = new DebugShader(mapName);
                    }
                    else {
                        std::cout << "Shader class " << mapName << " not registered!" << std::endl;
                    }
                }
            }
        }
        closedir(dir);
    }
    else
    {
        std::cout << "Problem reading from directory!" << std::endl;
    }
}

