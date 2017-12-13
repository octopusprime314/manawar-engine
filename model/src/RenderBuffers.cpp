#include "RenderBuffers.h"

RenderBuffers::RenderBuffers(){

}
RenderBuffers::~RenderBuffers(){

}

std::vector<Vector4>* RenderBuffers::getVertices() {
    return &_vertices;
}

std::vector<Vector4>* RenderBuffers::getNormals() {
    return &_normals;
}

std::vector<Texture2>* RenderBuffers::getTextures() {
    return &_textures;
}

std::vector<Vector4>* RenderBuffers::getDebugNormals(){
    return &_debugNormals;
}

void RenderBuffers::addVertex(Vector4 vertex) {
    _vertices.push_back(vertex);
}

void RenderBuffers::addNormal(Vector4 normal) {
    _normals.push_back(normal);
}

void RenderBuffers::addTexture(Texture2 texture){
    _textures.push_back(texture);
}

void RenderBuffers::addDebugNormal(Vector4 normal) {
    _debugNormals.push_back(normal);
}

void RenderBuffers::setVertexIndices(std::vector<int> indices) {
    _indices = indices;
}

void RenderBuffers::addVertexIndices(std::vector<int> indices) {
    _indices.insert(_indices.end(), indices.begin(), indices.end());
}

std::vector<int>* RenderBuffers::getIndices() {
    return &_indices;
}

