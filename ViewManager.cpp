#include "ViewManager.h"


void ViewManager::applyTransform(Matrix transform) {
    
    _view = _view * transform;
}