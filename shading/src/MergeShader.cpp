#include "MergeShader.h"
#include "GLIncludes.h"
#include "MRTFrameBuffer.h"
#include "MVP.h"
#include "GLSLShader.h"
#include "HLSLShader.h"
#include "EngineManager.h"
#include "DXLayer.h"

MergeShader::MergeShader() {
    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {
        _shader = new GLSLShader("mergeShader");
        glGenVertexArrays(1, &_dummyVAO);
    }
    else {
        std::vector<DXGI_FORMAT>* formats = new std::vector<DXGI_FORMAT>();
        formats->push_back(DXGI_FORMAT_R8G8B8A8_UNORM);
        _shader = new HLSLShader("mergeShader", "", formats);


        //struct Vertex {
        //    float pos[3];
        //    float uv[2];
        //};

        ////Now flatten vertices and normals out 
        //auto triBuffSize = 20;
        //auto flattenAttribs = new float[triBuffSize]; //Only include the x y and z values not w
        //auto flattenIndexes = new uint32_t[18]; //Only include the x y and z values not w, same size as vertices

        //flattenAttribs[0] = -1.0;
        //flattenAttribs[1] = -1.0;
        //flattenAttribs[2] = 0.0;

        //flattenAttribs[3] = 0.0;
        //flattenAttribs[4] = 0.0;

        //flattenAttribs[5] = 1.0;
        //flattenAttribs[6] = 1.0;
        //flattenAttribs[7] = 0.0;

        //flattenAttribs[8] = 1.0;
        //flattenAttribs[9] = 1.0;

        //flattenAttribs[10] = 1.0;
        //flattenAttribs[11] = -1.0;
        //flattenAttribs[12] = 0.0;

        //flattenAttribs[13] = 1.0;
        //flattenAttribs[14] = 0.0;

        //flattenAttribs[15] = -1.0;
        //flattenAttribs[16] = 1.0;
        //flattenAttribs[17] = 0.0;

        //flattenAttribs[18] = 0.0;
        //flattenAttribs[19] = 1.0;


        //flattenIndexes[0] = 0;
        //flattenIndexes[1] = 1;
        //flattenIndexes[2] = 2;
        //flattenIndexes[3] = 9;
        //flattenIndexes[4] = 10;
        //flattenIndexes[5] = 11;
        //flattenIndexes[6] = 3;
        //flattenIndexes[7] = 4;
        //flattenIndexes[8] = 5;

        //flattenIndexes[9] = 0;
        //flattenIndexes[10] = 1;
        //flattenIndexes[11] = 2;
        //flattenIndexes[12] = 3;
        //flattenIndexes[13] = 4;
        //flattenIndexes[14] = 5;
        //flattenIndexes[15] = 6;
        //flattenIndexes[16] = 7;
        //flattenIndexes[17] = 8;

        //UINT byteSize = static_cast<UINT>((triBuffSize) * sizeof(float));

        //_vertexBuffer = new ResourceBuffer(flattenAttribs, byteSize,
        //    DXLayer::instance()->getCmdList(), DXLayer::instance()->getDevice());

        //_vbv.BufferLocation = _vertexBuffer->getGPUAddress();
        //_vbv.StrideInBytes = sizeof(Vertex);
        //_vbv.SizeInBytes = byteSize;

        //auto indexBytes = static_cast<UINT>((triBuffSize / 3) * sizeof(uint32_t));
        //_indexBuffer = new ResourceBuffer(flattenIndexes, indexBytes,
        //    DXLayer::instance()->getCmdList(), DXLayer::instance()->getDevice());

        //_ibv.BufferLocation = _indexBuffer->getGPUAddress();
        //_ibv.Format = DXGI_FORMAT_R32_UINT;
        //_ibv.SizeInBytes = indexBytes;

        //_vao = new VAO(_vbv, _ibv);
    }
}

MergeShader::~MergeShader() {

}

void MergeShader::runShader(Texture* deferredTexture, Texture* velocityTexture) {

    //LOAD IN SHADER
    _shader->bind(); //use context for loaded shader
    if (EngineManager::getGraphicsLayer() == GraphicsLayer::OPENGL) {

        glBindVertexArray(_dummyVAO);
    }
    else {
        _shader->bindAttributes(nullptr);
    }

    _shader->updateData("deferredTexture", GL_TEXTURE0, deferredTexture);
    //_shader->updateData("velocityTexture", GL_TEXTURE1, velocityTexture);

    _shader->draw(0, 1, 3);
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)4);

    _shader->unbindAttributes();

    _shader->unbind();
}
