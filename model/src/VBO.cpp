#include "VBO.h"
#include "Model.h"

VBO::VBO(){

}
VBO::~VBO(){

}

void VBO::setVertexContext(GLuint context){
    _vertexBufferContext = context;
}

void VBO::setNormalContext(GLuint context){
    _normalBufferContext = context;
}

void VBO::setTextureContext(GLuint context){
    _textureBufferContext = context;
}

void VBO::setNormalDebugContext(GLuint context){
    _debugNormalBufferContext = context;
}

GLuint VBO::getVertexContext() {
    return _vertexBufferContext;
}

GLuint VBO::getNormalContext() {
    return _normalBufferContext;
}

GLuint VBO::getTextureContext() {
    return _textureBufferContext;
}

GLuint VBO::getNormalDebugContext() {
    return _debugNormalBufferContext;
}

void VBO::createVBO(RenderBuffers* renderBuffers, ModelClass classId) {

    auto vertices = renderBuffers->getVertices();
    auto normals = renderBuffers->getNormals();
    auto textures = renderBuffers->getTextures();
    auto indices = renderBuffers->getIndices();
    auto debugNormals = renderBuffers->getDebugNormals();

     //Now flatten vertices and normals out for opengl
    size_t triBuffSize = 0;
    float* flattenVerts = nullptr; //Only include the x y and z values not w
    float* flattenNorms = nullptr; //Only include the x y and z values not w, same size as vertices
    size_t textureBuffSize = 0;
    float* flattenTextures = nullptr; //Only include the s and t
    size_t lineBuffSize = 0;
    float* flattenNormLines = nullptr; //Only include the x y and z values not w, flat line data

    if(classId == ModelClass::ModelType){

        //Now flatten vertices and normals out for opengl
        triBuffSize = vertices->size() * 3;
        flattenVerts = new float[triBuffSize]; //Only include the x y and z values not w
        flattenNorms = new float[triBuffSize]; //Only include the x y and z values not w, same size as vertices
        textureBuffSize = textures->size() * 2;
        flattenTextures = new float[textureBuffSize]; //Only include the s and t
        lineBuffSize = debugNormals->size() * 3;
        flattenNormLines = new float[lineBuffSize]; //Only include the x y and z values not w, flat line data
        int i = 0; //iterates through vertices indexes
        for (auto vertex : *vertices) {
            float *flat = vertex.getFlatBuffer();
            flattenVerts[i++] = flat[0];
            flattenVerts[i++] = flat[1];
            flattenVerts[i++] = flat[2];
        }
        i = 0; //Reset for normal indexes
        for (auto normal : *normals) {
            float *flat = normal.getFlatBuffer();
            flattenNorms[i++] = flat[0];
            flattenNorms[i++] = flat[1];
            flattenNorms[i++] = flat[2];
        }
        i = 0; //Reset for texture indexes
        for (auto texture : *textures) {
            float *flat = texture.getFlatBuffer();
            flattenTextures[i++] = flat[0];
            flattenTextures[i++] = flat[1];
        }
        i = 0; //Reset for normal line indexes
        for (auto normalLine : *debugNormals) {
            float *flat = normalLine.getFlatBuffer();
            flattenNormLines[i++] = flat[0];
            flattenNormLines[i++] = flat[1];
            flattenNormLines[i++] = flat[2];
        }
    }
    else if(classId == ModelClass::AnimatedModelType){

        //Now flatten vertices and normals out for opengl
        triBuffSize = indices->size() * 3;
        flattenVerts = new float[triBuffSize]; //Only include the x y and z values not w
        flattenNorms = new float[triBuffSize]; //Only include the x y and z values not w, same size as vertices
        textureBuffSize = textures->size() * 2;
        flattenTextures = new float[textureBuffSize]; //Only include the s and t
        lineBuffSize = indices->size() * 2 * 3;
        flattenNormLines = new float[lineBuffSize]; //Only include the x y and z values not w, flat line data
        int i = 0; //iterates through vertices indexes
        for (auto index : *indices) {
            float *flat = (*vertices)[index].getFlatBuffer();
            flattenVerts[i++] = flat[0];
            flattenVerts[i++] = flat[1];
            flattenVerts[i++] = flat[2];
        }
        i = 0; //Reset for normal indexes
        for (auto index : *indices) {
            float *flat =  (*vertices)[index].getFlatBuffer();
            flattenNorms[i++] = flat[0];
            flattenNorms[i++] = flat[1];
            flattenNorms[i++] = flat[2];
        }
        i = 0; //Reset for texture indexes
        for (auto texture : *textures) {
            float *flat = texture.getFlatBuffer();
            flattenTextures[i++] = flat[0];
            flattenTextures[i++] = flat[1];
        }
        //i = 0; //Reset for normal line indexes
        //for (auto normalLine : _debugNormals) {
        //    float *flat = normalLine.getFlatBuffer();
        //    flattenNormLines[i++] = flat[0];
        //    flattenNormLines[i++] = flat[1];
        //    flattenNormLines[i++] = flat[2];
        //}
    }

    //Create a double buffer that will be filled with the vertex data
    glGenBuffers(1, &_vertexBufferContext);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferContext);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*triBuffSize, flattenVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Create a double buffer that will be filled with the normal data
    glGenBuffers(1, &_normalBufferContext);
    glBindBuffer(GL_ARRAY_BUFFER, _normalBufferContext);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*triBuffSize, flattenNorms, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Create a single buffer that will be filled with the texture coordinate data
    glGenBuffers(1, &_textureBufferContext); //Do not need to double buffer
    glBindBuffer(GL_ARRAY_BUFFER, _textureBufferContext);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*textureBuffSize, flattenTextures, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Create a double buffer that will be filled with the normal line data for visualizing normals
    glGenBuffers(1, &_debugNormalBufferContext);
    glBindBuffer(GL_ARRAY_BUFFER, _debugNormalBufferContext);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*lineBuffSize, flattenNormLines, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    delete[] flattenVerts;
    delete[] flattenNorms;
    delete[] flattenTextures;
    delete[] flattenNormLines;
}