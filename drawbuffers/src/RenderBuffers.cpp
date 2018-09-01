#include "RenderBuffers.h"

RenderBuffers::RenderBuffers() {

}
RenderBuffers::~RenderBuffers() {

}

std::vector<Vector4>* RenderBuffers::getVertices() {
    return &_vertices;
}

std::vector<Vector4>* RenderBuffers::getNormals() {
    return &_normals;
}

std::vector<Tex2>* RenderBuffers::getTextures() {
    return &_textures;
}

std::vector<Vector4>* RenderBuffers::getDebugNormals() {
    return &_debugNormals;
}

std::vector<Matrix>* RenderBuffers::getWorldSpaceTranforms() {
    return &_worldSpaceTransforms;
}

void RenderBuffers::addVertex(Vector4 vertex) {
    _vertices.push_back(vertex);
}

void RenderBuffers::addNormal(Vector4 normal) {
    _normals.push_back(normal);
}

void RenderBuffers::addTexture(Tex2 texture) {
    _textures.push_back(texture);
}

void RenderBuffers::addWorldSpaceTransform(Matrix worldSpaceTransform) {
    _worldSpaceTransforms.push_back(worldSpaceTransform);
}

void RenderBuffers::addDebugNormal(Vector4 normal) {
    _debugNormals.push_back(normal);
}

void RenderBuffers::addTextureMapIndex(int textureMapIndex) {
    _textureMapIndices.push_back(textureMapIndex);
}
void RenderBuffers::addTextureMapName(std::string textureMapName) {
    //Discard if it already exists
    for (auto textureName : _textureMapNames) {
        if (textureMapName.compare(textureName) == 0) {
            return;
        }
    }
    _textureMapNames.push_back(textureMapName);
}

int RenderBuffers::getTextureMapIndex(std::string textureMapName) {
    int index = 0;
    for (auto textureName : _textureMapNames) {
        if (textureMapName.compare(textureName) == 0) {
            return index;
        }
        index++;
    }
    return -1;
}

std::vector<int>* RenderBuffers::getTextureMapIndices() {
    return &_textureMapIndices;
}

std::vector<std::string>* RenderBuffers::getTextureMapNames() {
    return &_textureMapNames;
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
void RenderBuffers::clearBuffers() {
    _vertices.resize(0);
    _normals.resize(0);
    _textures.resize(0);
    _debugNormals.resize(0);
    _indices.resize(0);
}