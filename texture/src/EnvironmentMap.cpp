#include "EnvironmentMap.h"
#include "Entity.h"

EnvironmentMap::EnvironmentMap(GLuint width, GLuint height) :
    CubeMapRenderer(width, height, false),
    _environmentShader("environmentShader")/*,
    _animatedShader("animatedShader")*/ {

}

EnvironmentMap::~EnvironmentMap() {

}

void EnvironmentMap::render(std::vector<Entity*> entityList, MVP* mvp) {

    //Prepare cube face transforms
    preCubeFaceRender(entityList, mvp);

    for (Entity* entity : entityList) {
        if (entity->getModel()->getClassType() == ModelClass::ModelType) {
            _environmentShader.runShader(entity, _transforms);
        }
    }

    //Clean up cube face render contexts
    postCubeFaceRender();
}
