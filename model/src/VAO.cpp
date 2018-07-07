#include "VAO.h"
#include "Model.h"
#include "GeometryBuilder.h"

VAO::VAO() {

}
VAO::~VAO() {

}

void VAO::setVertexContext(GLuint context) {
    _vertexBufferContext = context;
}

void VAO::setNormalContext(GLuint context) {
    _normalBufferContext = context;
}

void VAO::setTextureContext(GLuint context) {
    _textureBufferContext = context;
}

void VAO::setNormalDebugContext(GLuint context) {
    _debugNormalBufferContext = context;
}

GLuint VAO::getVertexContext() {
    return _vertexBufferContext;
}

GLuint VAO::getNormalContext() {
    return _normalBufferContext;
}

GLuint VAO::getTextureContext() {
    return _textureBufferContext;
}

GLuint VAO::getNormalDebugContext() {
    return _debugNormalBufferContext;
}

GLuint VAO::getVertexLength() {
    return _vertexLength;
}

void VAO::createVAO(std::vector<Cube>* cubes) {
    
    float* flattenVerts = GeometryBuilder::buildCubes(cubes, _vertexLength);

    //Create a double buffer that will be filled with the vertex data
    glGenBuffers(1, &_vertexBufferContext);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferContext);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*_vertexLength * 3, flattenVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Bind regular vertex, normal and texture coordinate vao
    glGenVertexArrays(1, &_vaoContext);
    glBindVertexArray(_vaoContext);

    //Bind vertex buff context to current buffer
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferContext);

    //Say that the vertex data is associated with attribute 0 in the context of a shader program
    //Each vertex contains 3 floats per vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //Now enable vertex buffer at location 0
    glEnableVertexAttribArray(0);

    //Close vao
    glBindVertexArray(0);


    delete[] flattenVerts;
}


void VAO::createVAO(std::vector<Triangle>* triangles) {
    
    float* flattenVerts = GeometryBuilder::buildTriangles(triangles, _vertexLength);
    
    //Create a double buffer that will be filled with the vertex data
    glGenBuffers(1, &_vertexBufferContext);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferContext);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*_vertexLength * 3, flattenVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Bind regular vertex, normal and texture coordinate vao
    glGenVertexArrays(1, &_vaoContext);
    glBindVertexArray(_vaoContext);

    //Bind vertex buff context to current buffer
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferContext);

    //Say that the vertex data is associated with attribute 0 in the context of a shader program
    //Each vertex contains 3 floats per vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //Now enable vertex buffer at location 0
    glEnableVertexAttribArray(0);

    //Close vao
    glBindVertexArray(0);

   
    delete[] flattenVerts;
}

void VAO::createVAO(std::vector<Sphere>* spheres) {

    float* flattenVerts = GeometryBuilder::buildSpheres(spheres, _vertexLength);

    //Create a double buffer that will be filled with the vertex data
    glGenBuffers(1, &_vertexBufferContext);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferContext);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*_vertexLength * 3, flattenVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Bind regular vertex, normal and texture coordinate vao
    glGenVertexArrays(1, &_vaoContext);
    glBindVertexArray(_vaoContext);

    //Bind vertex buff context to current buffer
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferContext);

    //Say that the vertex data is associated with attribute 0 in the context of a shader program
    //Each vertex contains 3 floats per vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //Now enable vertex buffer at location 0
    glEnableVertexAttribArray(0);

    //Close vao
    glBindVertexArray(0);

    delete[] flattenVerts;
}

void VAO::createVAO(RenderBuffers* renderBuffers, ModelClass classId, Animation* animation) {
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

    if (classId == ModelClass::ModelType) {

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
    else if (classId == ModelClass::AnimatedModelType) {

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
            float *flat = (*normals)[index].getFlatBuffer();
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

        auto boneIndexes = animation->getBoneIndexes();
        auto boneWeights = animation->getBoneWeights();

        //Now flatten bone indexes and weights out for opengl
        std::vector<int>* indices = renderBuffers->getIndices();
        size_t boneIndexesBuffSize = indices->size() * 8;
        float* flattenIndexes = new float[boneIndexesBuffSize];
        float* flattenWeights = new float[boneIndexesBuffSize];

        i = 0; //iterates through vertices indexes
        for (auto vertexIndex : *indices) {
            auto indexes = (*boneIndexes)[vertexIndex];
            flattenIndexes[i++] = static_cast<float>(indexes[0]);
            flattenIndexes[i++] = static_cast<float>(indexes[1]);
            flattenIndexes[i++] = static_cast<float>(indexes[2]);
            flattenIndexes[i++] = static_cast<float>(indexes[3]);
            flattenIndexes[i++] = static_cast<float>(indexes[4]);
            flattenIndexes[i++] = static_cast<float>(indexes[5]);
            flattenIndexes[i++] = static_cast<float>(indexes[6]);
            flattenIndexes[i++] = static_cast<float>(indexes[7]);
        }
        i = 0; //Reset for normal indexes
        for (auto vertexIndex : *indices) {
            auto weights = (*boneWeights)[vertexIndex];
            flattenWeights[i++] = weights[0];
            flattenWeights[i++] = weights[1];
            flattenWeights[i++] = weights[2];
            flattenWeights[i++] = weights[3];
            flattenWeights[i++] = weights[4];
            flattenWeights[i++] = weights[5];
            flattenWeights[i++] = weights[6];
            flattenWeights[i++] = weights[7];
        }


        glGenBuffers(1, &_indexContext);
        glBindBuffer(GL_ARRAY_BUFFER, _indexContext); //Load in vertex buffer context
                                                      //Load the vertex data into the current vertex context
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*boneIndexesBuffSize, flattenIndexes, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind

        glGenBuffers(1, &_weightContext);
        glBindBuffer(GL_ARRAY_BUFFER, _weightContext); //Load in normal buffer context
                                                       //Load the normal data into the current normal context
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*boneIndexesBuffSize, flattenWeights, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind

        delete[] flattenIndexes;
        delete[] flattenWeights;
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

    //Bind regular vertex, normal and texture coordinate vao
    glGenVertexArrays(1, &_vaoContext);
    glBindVertexArray(_vaoContext);

    //Bind vertex buff context to current buffer
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferContext);

    //Say that the vertex data is associated with attribute 0 in the context of a shader program
    //Each vertex contains 3 floats per vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //Now enable vertex buffer at location 0
    glEnableVertexAttribArray(0);

    //Bind normal buff context to current buffer
    glBindBuffer(GL_ARRAY_BUFFER, _normalBufferContext);

    //Say that the normal data is associated with attribute 1 in the context of a shader program
    //Each normal contains 3 floats per normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //Now enable normal buffer at location 1
    glEnableVertexAttribArray(1);

    //Bind texture coordinate buff context to current buffer
    glBindBuffer(GL_ARRAY_BUFFER, _textureBufferContext);

    //Say that the texture coordinate data is associated with attribute 2 in the context of a shader program
    //Each texture coordinate contains 2 floats per texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    //Now enable texture buffer at location 2
    glEnableVertexAttribArray(2);

    if (classId == ModelClass::AnimatedModelType) {
        //Bind bone index buff context to current buffer
        glBindBuffer(GL_ARRAY_BUFFER, _indexContext);

        //First 4 indexes
        //Specify stride to be 8 because the beginning of each index attribute value is 8 bytes away
        //Say that the bone index data is associated with attribute 3 in the context of a shader program
        //Each bone index contains 4 floats per index
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), 0);

        //Now enable bone index buffer at location 1
        glEnableVertexAttribArray(3);

        //Second 4 indexes
        //Specify stride to be 8 because the beginning of each index attribute value is 8 bytes away
        //Specify offset for attribute location of indexes2 to be 4 bytes offset from the beginning location of the buffer
        //Say that the bone index data is associated with attribute 3 in the context of a shader program
        //Each bone index contains 4 floats per index
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(4 * sizeof(GL_FLOAT)));

        //Now enable bone index buffer at location 2
        glEnableVertexAttribArray(4);

        //Bind weight buff context to current buffer
        glBindBuffer(GL_ARRAY_BUFFER, _weightContext);

        //First 4 weights
        //Specify stride to be 8 because the beginning of each weight attribute value is 8 bytes away
        //Say that the weight data is associated with attribute 4 in the context of a shader program
        //Each weight contains 4 floats per index
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), 0);

        //Now enable weight buffer at location 3
        glEnableVertexAttribArray(5);

        //Second 4 weights
        //Specify stride to be 8 because the beginning of each weight attribute value is 8 bytes away
        //Specify offset for attribute location of weights2 to be 4 bytes offset from the beginning location of the buffer
        //Say that the weight data is associated with attribute 4 in the context of a shader program
        //Each weight contains 4 floats per index
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(4 * sizeof(GL_FLOAT)));

        //Now enable weight buffer at location 4
        glEnableVertexAttribArray(6);
    }

    //Close vao
    glBindVertexArray(0);

    //Bind vertex vao only for shadow pass
    glGenVertexArrays(1, &_vaoShadowContext);
    glBindVertexArray(_vaoShadowContext);

    //Bind vertex buff context to current buffer
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferContext);

    //Say that the vertex data is associated with attribute 0 in the context of a shader program
    //Each vertex contains 3 floats per vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //Now enable vertex buffer at location 0
    glEnableVertexAttribArray(0);

    if (classId == ModelClass::AnimatedModelType) {
        //Bind bone index buff context to current buffer
        glBindBuffer(GL_ARRAY_BUFFER, _indexContext);

        //First 4 indexes
        //Specify stride to be 8 because the beginning of each index attribute value is 8 bytes away
        //Say that the bone index data is associated with attribute 3 in the context of a shader program
        //Each bone index contains 4 floats per index
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), 0);

        //Now enable bone index buffer at location 1
        glEnableVertexAttribArray(1);

        //Second 4 indexes
        //Specify stride to be 8 because the beginning of each index attribute value is 8 bytes away
        //Specify offset for attribute location of indexes2 to be 4 bytes offset from the beginning location of the buffer
        //Say that the bone index data is associated with attribute 3 in the context of a shader program
        //Each bone index contains 4 floats per index
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(4 * sizeof(GL_FLOAT)));

        //Now enable bone index buffer at location 2
        glEnableVertexAttribArray(2);

        //Bind weight buff context to current buffer
        glBindBuffer(GL_ARRAY_BUFFER, _weightContext);

        //First 4 weights
        //Specify stride to be 8 because the beginning of each weight attribute value is 8 bytes away
        //Say that the weight data is associated with attribute 4 in the context of a shader program
        //Each weight contains 4 floats per index
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), 0);

        //Now enable weight buffer at location 3
        glEnableVertexAttribArray(3);

        //Second 4 weights
        //Specify stride to be 8 because the beginning of each weight attribute value is 8 bytes away
        //Specify offset for attribute location of weights2 to be 4 bytes offset from the beginning location of the buffer
        //Say that the weight data is associated with attribute 4 in the context of a shader program
        //Each weight contains 4 floats per index
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)(4 * sizeof(GL_FLOAT)));

        //Now enable weight buffer at location 4
        glEnableVertexAttribArray(4);
    }

    //Close vao
    glBindVertexArray(0);

    delete[] flattenVerts;
    delete[] flattenNorms;
    delete[] flattenTextures;
    delete[] flattenNormLines;

}

GLuint VAO::getVAOContext() {
    return _vaoContext;
}

GLuint VAO::getVAOShadowContext() {
    return _vaoShadowContext;
}
