
#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>
#include "font.h"
#include "IOEventDistributor.h"
#include "TextureBroker.h"

const float ratio = static_cast<float>(IOEventDistributor::screenPixelHeight) /
                                       IOEventDistributor::screenPixelWidth;

struct vec2 {
    float x;
    float y;
};

struct vec3 {
    float x;
    float y;
    float z;
};

void parseFontFile(std::string& filename,
                   FontInfo&    out) {
    std::ifstream filestream(filename);
    std::string   line;
    std::string   token;
    std::size_t   i;
    std::string   key;
    std::string   value;

    while (std::getline(filestream, line)) {
        std::stringstream ss;
        ss << line;
        ss >> token;

        if (token == "chars") {
            ss >> token;
            i             = token.find('=');
            key           = token.substr(0, i);
            value         = token.substr(i + 1);
            out.charCount = atoi(value.c_str());
        }
        else if (token == "char") {
            uint32_t id = 0;

            std::stringstream converter;
            ss >> token;
            i     = token.find('=');
            key   = token.substr(0, i);
            value = token.substr(i + 1);
            converter << value;

            assert(key == "id");

            converter >> id;
            out.chars[id].id = id;

            while (ss.eof() == false) {
                converter.str(std::string());
                converter.clear();
                ss >> token;
                i     = token.find('=');
                key   = token.substr(0, i);
                value = token.substr(i + 1);
                converter << value;

                if (key == "x") {
                    converter >> out.chars[id].x;
                }
                else if (key == "y") {
                    converter >> out.chars[id].y;
                }
                else if (key == "width") {
                    converter >> out.chars[id].width;
                }
                else if (key == "height") {
                    converter >> out.chars[id].height;
                }
                else if (key == "xoffset") {
                    converter >> out.chars[id].xoffset;
                }
                else if (key == "yoffset") {
                    converter >> out.chars[id].yoffset;
                }
                else if (key == "xadvance") {
                    converter >> out.chars[id].xadvance;
                }
                else if (key == "page") {
                    converter >> out.chars[id].page;
                }
                else if (key == "chnl") {
                    converter >> out.chars[id].chnl;
                }
            }
        }
    }
}

FontRenderer::FontRenderer(std::string fileName)
    : _fontShader(static_cast<FontShader*>(ShaderBroker::instance()->getShader("fontShader"))) {
    std::string fullPath = FONT_LOCATION + fileName;
    parseFontFile(fullPath, _fontInfo);

    TextureBroker* pTb   = TextureBroker::instance();
    pTb->addTexture("../assets/textures/font/ubuntu_mono_regular_0.png");

    // create vertex buffer
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * _bufferSize * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &_texCoordsVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _texCoordsVbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * _bufferSize * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, _texCoordsVbo);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void FontRenderer::drawFont(float x, float y, std::string s, uint64_t timeDelta) {
    float cursorAdvance = 0;
    float scale = 700;

    // convert to ndc
    x -= 1;
    y += 1;

    static vec3 vb[65536];
    static vec2 tcb[65536];

    vec3* pvb = vb;
    vec2* ptcb = tcb;

    for (int i = 0; i < s.size(); i++) {
        uint32_t asciiVal = s[i];

        // generate character position 
        float xoffset = _fontInfo.chars[asciiVal].xoffset / scale;
        float yoffset = _fontInfo.chars[asciiVal].yoffset / scale;
        float width = _fontInfo.chars[asciiVal].width / scale;
        float height = _fontInfo.chars[asciiVal].height / scale;
        float advance = _fontInfo.chars[asciiVal].xadvance / scale;

        vec3 topLeftPos = { x + xoffset,         y - yoffset,          0 };
        vec3 topRightPos = { x + xoffset + width, y - yoffset,          0 };
        vec3 bottomLeftPos = { x + xoffset        , y - yoffset - height, 0 };
        vec3 bottomRightPos = { x + xoffset + width, y - yoffset - height, 0 };

        vec3 cursorPosition[] = { bottomLeftPos,
                                  topLeftPos,
                                  topRightPos,
                                  topRightPos,
                                  bottomRightPos,
                                  bottomLeftPos };

        cursorPosition[0].x += cursorAdvance;
        cursorPosition[1].x += cursorAdvance;
        cursorPosition[2].x += cursorAdvance;
        cursorPosition[3].x += cursorAdvance;
        cursorPosition[4].x += cursorAdvance;
        cursorPosition[5].x += cursorAdvance;


        // generate text coordinates
        vec2 bottomLeft = { _fontInfo.chars[asciiVal].x / 256.0f,
                           1.0f - (_fontInfo.chars[asciiVal].y + _fontInfo.chars[asciiVal].height) / 256.0f };
        vec2 topRight = { (_fontInfo.chars[asciiVal].x + _fontInfo.chars[asciiVal].width) / 256.0f,
                           1.0f - _fontInfo.chars[asciiVal].y / 256.0f };
        vec2 topLeft = { _fontInfo.chars[asciiVal].x / 256.0f,
                           1.0f - (_fontInfo.chars[asciiVal].y) / 256.0f };
        vec2 bottomRight = { (_fontInfo.chars[asciiVal].x + _fontInfo.chars[asciiVal].width) / 256.0f,
                           1.0f - (_fontInfo.chars[asciiVal].y + _fontInfo.chars[asciiVal].height) / 256.0f };

        vec2 texCoords[] = { bottomLeft, topLeft, topRight, topRight, bottomRight, bottomLeft };

        // copy quad
        memcpy_s(pvb, sizeof(cursorPosition), cursorPosition, sizeof(cursorPosition));

        memcpy_s(ptcb, sizeof(texCoords), texCoords, sizeof(texCoords));
        size_t increment = sizeof(cursorPosition);
        pvb += 6;
        ptcb += 6;

        cursorAdvance += advance - xoffset;
    }

    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glCheck();

    float* pData = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    glCheck();

    if (pData) {
        memcpy_s(pData, 18 * sizeof(float) * s.size(), vb, 18 * sizeof(float) * s.size());
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glCheck();
    }

    glBindBuffer(GL_ARRAY_BUFFER, _texCoordsVbo);
    glCheck();
    pData = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    if (pData) {
        memcpy_s(pData, 12 * sizeof(float) * s.size(), tcb, 12 * sizeof(float) * s.size());
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glCheck();

    _fontShader->runShader(_vao, s);
}