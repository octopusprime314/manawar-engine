#include "VAO.h"
#include "Model.h"
#include "GeometryBuilder.h"
#include "EngineManager.h"
#include "DXLayer.h"

VAO::VAO() {

}
VAO::~VAO() {

}

VAO::VAO(D3D12_VERTEX_BUFFER_VIEW vbv, D3D12_INDEX_BUFFER_VIEW ibv) {
    _vbv = vbv;
    _ibv = ibv;
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

D3D12_INDEX_BUFFER_VIEW VAO::getIndexBuffer() {
    return _ibv;
}

D3D12_VERTEX_BUFFER_VIEW VAO::getVertexBuffer() {
    return _vbv;
}

ResourceBuffer* VAO::getIndexResource() {
    return _indexBuffer;
}

ResourceBuffer* VAO::getVertexResource() {
    return _vertexBuffer;
}

void VAO::addTextureStride(std::pair<std::string, int> stride) {
    _textureStride.push_back(stride);
}

TextureMetaData VAO::getTextureStrides() {
    return _textureStride;
}

void VAO::setPrimitiveOffsetId(GLuint id) {
    _primitiveOffsetId = id;
}

GLuint VAO::getPrimitiveOffsetId() {
    return _primitiveOffsetId;
}

GLuint VAO::getVAOContext() {
    return _vaoContext;
}

GLuint VAO::getVAOShadowContext() {
    return _vaoShadowContext;
}

void VAO::_buildVertices(float* flattenVerts) {
    
    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {

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
    }
    else if (EngineManager::getGraphicsLayer() >= GraphicsLayer::DX12) {
        //IMPLEMENT ME!!!!
    }
}


void VAO::createVAO(std::vector<Cube>* cubes, GeometryConstruction geometryType) {
    
    float* flattenVerts = GeometryBuilder::buildCubes(cubes, _vertexLength, geometryType);
    _buildVertices(flattenVerts);
    delete[] flattenVerts;
}

void VAO::createVAO(std::vector<Triangle>* triangles) {
    
    float* flattenVerts = GeometryBuilder::buildTriangles(triangles, _vertexLength);
    _buildVertices(flattenVerts);
    delete[] flattenVerts;
}

void VAO::createVAO(std::vector<Sphere>* spheres, GeometryConstruction geometryType) {

    float* flattenVerts = GeometryBuilder::buildSpheres(spheres, _vertexLength, geometryType);
    _buildVertices(flattenVerts);
    delete[] flattenVerts;
}

void VAO::createVAO(RenderBuffers* renderBuffers, ModelClass classId, Animation* animation) {
    
    size_t    triBuffSize     = 0;
    float*    flattenVerts    = nullptr;
    float*    flattenNorms    = nullptr;
    size_t    textureBuffSize = 0;
    float*    flattenTextures = nullptr;
    float*    flattenAttribs  = nullptr;
    uint32_t* flattenIndexes  = nullptr;
    auto      vertices        = renderBuffers->getVertices();
    auto      normals         = renderBuffers->getNormals();
    auto      textures        = renderBuffers->getTextures();
    auto      indices         = renderBuffers->getIndices();

    _vertexLength = static_cast<GLuint>(vertices->size());

    if (classId == ModelClass::ModelType) {

        if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {

            triBuffSize     = vertices->size() * 3;
            flattenVerts    = new float[triBuffSize];
            flattenNorms    = new float[triBuffSize];
            textureBuffSize = textures->size() * 2;
            flattenTextures = new float[textureBuffSize];
            
            int i = 0; //iterates through vertices indexes
            for (auto vertex : *vertices) {
                float *flat       = vertex.getFlatBuffer();
                flattenVerts[i++] = flat[0];
                flattenVerts[i++] = flat[1];
                flattenVerts[i++] = flat[2];
            }
            i = 0; //Reset for normal indexes
            for (auto normal : *normals) {
                float *flat       = normal.getFlatBuffer();
                flattenNorms[i++] = flat[0];
                flattenNorms[i++] = flat[1];
                flattenNorms[i++] = flat[2];
            }
            i = 0; //Reset for texture indexes
            for (auto texture : *textures) {
                float *flat          = texture.getFlatBuffer();
                flattenTextures[i++] = flat[0];
                flattenTextures[i++] = flat[1];
            }
        }
        else {

            //Now flatten vertices and normals out 
            triBuffSize    = vertices->size() * 3;
            flattenAttribs = new float[triBuffSize + (normals->size() * 3) + 
                                      (textures->size() * 2)]; 
            flattenIndexes = new uint32_t[triBuffSize / 3]; 

            int i = 0; //iterates through vertices indexes
            uint32_t j = 0;
            for (auto vertex : *vertices) {
                float *flatVert     = vertex.getFlatBuffer();
                flattenAttribs[i++] = flatVert[0];
                flattenAttribs[i++] = flatVert[1];
                flattenAttribs[i++] = flatVert[2];
                flattenIndexes[j]   = j;
                
                j++;
                i += 5;
            }

            i = 3;
            for (auto normal : *normals) {
                float *flatNormal   = normal.getFlatBuffer();
                flattenAttribs[i++] = flatNormal[0];
                flattenAttribs[i++] = flatNormal[1];
                flattenAttribs[i++] = flatNormal[2];
                i += 5;
            }

            i = 6;
            for (auto texture : *textures) {
                float *flat         = texture.getFlatBuffer();
                flattenAttribs[i++] = flat[0];
                flattenAttribs[i++] = flat[1];
                i += 6;
            }
        }
    }
    else if (classId == ModelClass::AnimatedModelType) {
        
        int i = 0;

        if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
            triBuffSize     = indices->size() * 3;
            flattenVerts    = new float[triBuffSize];
            flattenNorms    = new float[triBuffSize];
            textureBuffSize = textures->size() * 2;
            flattenTextures = new float[textureBuffSize];

            for (auto index : *indices) {
                float *flat       = (*vertices)[index].getFlatBuffer();
                flattenVerts[i++] = flat[0];
                flattenVerts[i++] = flat[1];
                flattenVerts[i++] = flat[2];
            }
            i = 0; //Reset for normal indexes
            for (auto index : *indices) {
                float *flat       = (*normals)[index].getFlatBuffer();
                flattenNorms[i++] = flat[0];
                flattenNorms[i++] = flat[1];
                flattenNorms[i++] = flat[2];
            }
            i = 0; //Reset for texture indexes
            for (auto texture : *textures) {
                float *flat          = texture.getFlatBuffer();
                flattenTextures[i++] = flat[0];
                flattenTextures[i++] = flat[1];
            }
        }
        else {

            //Now flatten vertices and normals out 
            triBuffSize    = indices->size() * 3;
            flattenAttribs = new float[triBuffSize + (indices->size() * 3) +
                                      (textures->size() * 2)];
            flattenIndexes = new uint32_t[triBuffSize / 3];

            uint16_t j = 0;
            i = 0;
            for (auto index : *indices) {
                float *flatVert     = (*vertices)[index].getFlatBuffer();
                flattenAttribs[i++] = flatVert[0];
                flattenAttribs[i++] = flatVert[1];
                flattenAttribs[i++] = flatVert[2];
                flattenIndexes[j]   = j;
                
                j++;
                i += 5;
            }

            i = 3;
            for (auto index : *indices) {
                float *flatNormal   = (*normals)[index].getFlatBuffer();
                flattenAttribs[i++] = flatNormal[0];
                flattenAttribs[i++] = flatNormal[1];
                flattenAttribs[i++] = flatNormal[2];
                i += 5;
            }

            i = 6;
            for (auto texture : *textures) {
                float *flat         = texture.getFlatBuffer();
                flattenAttribs[i++] = flat[0];
                flattenAttribs[i++] = flat[1];
                i += 6;
            }
        }

        auto boneIndexes           = animation->getBoneIndexes();
        auto boneWeights           = animation->getBoneWeights();
        std::vector<int>* indices  = renderBuffers->getIndices();
        size_t boneIndexesBuffSize = indices->size() * 8;
        float* flattenIndexes      = new float[boneIndexesBuffSize];
        float* flattenWeights      = new float[boneIndexesBuffSize];

        i = 0; //iterates through vertices indexes
        for (auto vertexIndex : *indices) {
            auto indexes        = (*boneIndexes)[vertexIndex];
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
            auto weights        = (*boneWeights)[vertexIndex];
            flattenWeights[i++] = weights[0];
            flattenWeights[i++] = weights[1];
            flattenWeights[i++] = weights[2];
            flattenWeights[i++] = weights[3];
            flattenWeights[i++] = weights[4];
            flattenWeights[i++] = weights[5];
            flattenWeights[i++] = weights[6];
            flattenWeights[i++] = weights[7];
        }

        if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {

            glGenBuffers(1, &_indexContext);
            glBindBuffer(GL_ARRAY_BUFFER, _indexContext);

            glBufferData(GL_ARRAY_BUFFER,
                         sizeof(float)*boneIndexesBuffSize,
                         flattenIndexes,
                         GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glGenBuffers(1, &_weightContext);
            glBindBuffer(GL_ARRAY_BUFFER, _weightContext);

            glBufferData(GL_ARRAY_BUFFER,
                         sizeof(float)*boneIndexesBuffSize,
                         flattenWeights,
                         GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else {
            //IMPLEMENT ME!!!
        }

        delete[] flattenIndexes;
        delete[] flattenWeights;
    }

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
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
    }
    else {

        struct Vertex {
            float pos[3];
            float normal[3];
            float uv[2];
        };

        UINT byteSize = 0;
        if (classId == ModelClass::AnimatedModelType) {
            byteSize = static_cast<UINT>((triBuffSize + (indices->size() * 3) + (textures->size() * 2)) * sizeof(float));
        }
        else {
            byteSize = static_cast<UINT>((triBuffSize + (normals->size() * 3) + (textures->size() * 2)) * sizeof(float));
        }

        auto indexBytes = static_cast<UINT>((triBuffSize / 3) * sizeof(uint32_t));

        _vertexBuffer       = new ResourceBuffer(flattenAttribs,
                                                 byteSize,
                                                 DXLayer::instance()->getCmdList(),
                                                 DXLayer::instance()->getDevice());

        _indexBuffer        = new ResourceBuffer(flattenIndexes,
                                                 indexBytes, 
                                                 DXLayer::instance()->getCmdList(),
                                                 DXLayer::instance()->getDevice());

        _vbv.BufferLocation = _vertexBuffer->getGPUAddress();
        _vbv.StrideInBytes  = sizeof(Vertex);
        _vbv.SizeInBytes    = byteSize;

        _ibv.BufferLocation = _indexBuffer->getGPUAddress();
        _ibv.Format         = DXGI_FORMAT_R32_UINT;
        _ibv.SizeInBytes    = indexBytes;
    }

    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {

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
    }
    else {
        //IMPLEMENT ME!!!
    }

    delete[] flattenVerts;
    delete[] flattenNorms;
    delete[] flattenTextures;

}

void VAO::createVAO(RenderBuffers* renderBuffers, int begin, int range) {

    
    size_t triBuffSize        = 0;
    float* flattenVerts       = nullptr;
    float* flattenNorms       = nullptr;
    size_t textureBuffSize    = 0;
    float* flattenTextures    = nullptr;
    size_t lineBuffSize       = 0;
    float* flattenNormLines   = nullptr;
    auto   vertices           = renderBuffers->getVertices();
    auto   normals            = renderBuffers->getNormals();
    auto   textures           = renderBuffers->getTextures();
    auto   indices            = renderBuffers->getIndices();
    auto   debugNormals       = renderBuffers->getDebugNormals();

   _vertexLength              = static_cast<GLuint>(vertices->size());
    triBuffSize               = range * 3;
    flattenVerts              = new float[triBuffSize];
    flattenNorms              = new float[triBuffSize];
    textureBuffSize           = range * 2;
    flattenTextures           = new float[textureBuffSize];

    int i = 0; //iterates through vertices indexes
    for (int j = begin; j < begin + range; j++) {

        float *flat       = (*vertices)[j].getFlatBuffer();
        flattenVerts[i++] = flat[0];
        flattenVerts[i++] = flat[1];
        flattenVerts[i++] = flat[2];
    }
    i = 0; //Reset for normal indexes
    for (int j = begin; j < begin + range; j++) {

        float *flat       = (*normals)[j].getFlatBuffer();
        flattenNorms[i++] = flat[0];
        flattenNorms[i++] = flat[1];
        flattenNorms[i++] = flat[2];
    }
    i = 0; //Reset for texture indexes
    for (int j = begin; j < begin + range; j++) {

        float *flat          = (*textures)[j].getFlatBuffer();
        flattenTextures[i++] = flat[0];
        flattenTextures[i++] = flat[1];
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

    //Close vao
    glBindVertexArray(0);

    delete[] flattenVerts;
    delete[] flattenNorms;
    delete[] flattenTextures;

}
