#include "RenderTexture.h"

RenderTexture::RenderTexture() {

}

RenderTexture::RenderTexture(GLuint width, GLuint height, TextureFormat format) :
    Texture("RenderTexture", width, height) {
    
    glGenTextures(1, &_textureContext);
    
    //Bind current texture context diffuse
    glBindTexture(GL_TEXTURE_2D, _textureContext);

    if (format == TextureFormat::RGBA_UNSIGNED_BYTE) {
        //spell out texture format, RGBA format but can use RGB, data pointer is null
        //because the frame buffer is responsible for allocating and populating texture data
        //Diffuse data will be in unsigned format with 8 bits each for RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    else if (format == TextureFormat::DEPTH32_FLOAT) {
        //spell out texture format, width x height texture, Depth 32 bit format, data pointer is null
        //because the frame buffer is responsible for allocating and populating texture data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    else if (format == TextureFormat::RGBA_FLOAT) {
        //spell out texture format, RGB format but can use RGB, data pointer is null
        //because the frame buffer is responsible for allocating and populating texture data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _width, _height, 0, GL_RGBA, GL_FLOAT, NULL);
    }
    else if (format == TextureFormat::R_FLOAT) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, _width, _height, 0, GL_RGB, GL_FLOAT, NULL);
    }
    else if (format == TextureFormat::R_UNSIGNED_BYTE) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }

    //texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //remove texture context
    glBindTexture(GL_TEXTURE_2D, 0);
}

RenderTexture::~RenderTexture() {

}

