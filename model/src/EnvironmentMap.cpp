#include "EnvironmentMap.h"

EnvironmentMap::EnvironmentMap(GLuint width, GLuint height) : 
    CubeMapRenderer(width, height, false),
    _environmentShader("environmentShader")/*,
    _animatedShader("animatedShader")*/ {

}

EnvironmentMap::~EnvironmentMap() {

}

void EnvironmentMap::render(std::vector<Model*> modelList, MVP* mvp) {
    
    //Prepare cube face transforms
    preCubeFaceRender(modelList, mvp);

    for (Model* model : modelList) {
        if (model->getClassType() == ModelClass::ModelType) {
            _environmentShader.runShader(model, _transforms);
        }
    }

    //Clean up cube face render contexts
    postCubeFaceRender();
}