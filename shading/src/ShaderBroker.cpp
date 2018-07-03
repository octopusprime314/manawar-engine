#include "ShaderBroker.h"
#include "Dirent.h"
#include "StaticShader.h"
#include "AnimationShader.h"
#include "EffectShader.h"
#include "ShadowStaticShader.h"
#include "SSAOShader.h"
#include "ShadowAnimatedPointShader.h"
#include "ShadowPointShader.h"
#include "MergeShader.h"
#include "ForwardShader.h"
#include "DeferredShader.h"
#include "FontShader.h"
#include "ComputeShader.h"
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

void ShaderBroker::compileShaders() {

    _gatherShaderNames();
}

Shader* ShaderBroker::getShader(std::string shaderName) {
    return _shaders[shaderName];
}

void ShaderBroker::recompileShader(std::string shaderName) {

    if (_shaders.find(shaderName) != _shaders.end()) {
        _shaders[shaderName] = new Shader(shaderName);
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
                    if (mapName == "staticShader") {
                        _shaders[mapName] = new StaticShader(mapName);
                    }
                    else if (mapName == "animatedShader") {
                        _shaders[mapName] = new AnimationShader(mapName);
                    }
                    else if (mapName == "animatedShadowShader") {
                        _shaders[mapName] = new ShadowAnimatedShader(mapName);
                    }
                    else if (mapName == "waterShader" || mapName == "fireShader") {
                        _shaders[mapName] = new EffectShader(mapName);
                    }
                    else if (mapName == "staticShadowShader") {
                        _shaders[mapName] = new ShadowStaticShader(mapName);
                    }
                    else if (mapName == "ssaoShader") {
                        _shaders[mapName] = new SSAOShader();
                    }
                    else if (mapName == "pointShadowShader") {
                        _shaders[mapName] = new ShadowPointShader(mapName);
                    }
                    else if (mapName == "pointAnimatedShadowShader") {
                        _shaders[mapName] = new ShadowAnimatedPointShader(mapName);
                    }
                    else if (mapName == "mergeShader") {
                        _shaders[mapName] = new MergeShader();
                    }
                    else if (mapName == "forwardShader") {
                        _shaders[mapName] = new ForwardShader("forwardShader");
                    }
                    else if (mapName == "deferredShader") {
                        _shaders[mapName] = new DeferredShader(mapName);
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
                        _shaders[mapName] = new ComputeShader(mapName);
                    }
                    else if (mapName == "fontShader") {
                        _shaders[mapName] = new FontShader(mapName);
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

