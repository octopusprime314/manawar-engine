#include "CubeMap.h"
#include "GLIncludes.h"

CubeMap::CubeMap(unsigned int width, unsigned int height) :
    _width(width),
    _height(height) {

    //Generate texture context
    glGenTextures(1, &_depthCubemap);

    //Bind the texture and create 6 sides of a texture cube
    glBindTexture(GL_TEXTURE_CUBE_MAP, _depthCubemap);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, _width, _height, 
            0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    //Texture params
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    //Generate frame buffer context and bind it
    glGenFramebuffers(1, &_cubeFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _cubeFrameBuffer);

    //Attach the framebuffer object to the cubemap texture
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthCubemap, 0);

    //Disables color buffer rendering for the depth only passage
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

}

CubeMap::~CubeMap() {

}

unsigned int CubeMap::getWidth() {
    return _width;
}
unsigned int CubeMap::getHeight() {
    return _height;
}

unsigned int CubeMap::getCubeDepthFrameBufferContext() {
    return _cubeFrameBuffer;
}

unsigned int CubeMap::getCubeDepthMapContext() {
    return _depthCubemap;
}