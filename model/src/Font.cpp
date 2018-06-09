
#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>
#include "font.h"
#include "TextureBroker.h"

struct vec2
{
    float x;
    float y;
};

struct vec3
{
    float x;
    float y;
    float z;
};

void parseFontFile(std::string& filename, FontInfo& out)
{
    std::ifstream filestream(filename);
    std::string line;
    std::string token;
    std::size_t i;
    std::string key;
    std::string value;
    while (std::getline(filestream, line))
    {
        std::stringstream ss;
        ss << line;
        ss >> token;

        if (token == "chars")
        {
            ss >> token;
            i = token.find('=');
            key = token.substr(0, i);
            value = token.substr(i + 1);
            out.charCount = atoi(value.c_str());
        }
        else if (token == "char")
        {
            uint32_t id = 0;

            std::stringstream converter;
            ss >> token;
            i = token.find('=');
            key = token.substr(0, i);
            value = token.substr(i + 1);
            converter << value;

            assert(key == "id");

            converter >> id;
            out.chars[id].id = id;

            while (ss.eof() == false)
            {
                converter.str(std::string());
                converter.clear();
                ss >> token;
                i = token.find('=');
                key = token.substr(0, i);
                value = token.substr(i + 1);
                converter << value;

                if (key == "x")
                {
                    converter >> out.chars[id].x;
                }
                else if (key == "y")
                {
                    converter >> out.chars[id].y;
                }
                else if (key == "width")
                {
                    converter >> out.chars[id].width;
                }
                else if (key == "height")
                {
                    converter >> out.chars[id].height;
                }
                else if (key == "xoffset")
                {
                    converter >> out.chars[id].xoffset;
                }
                else if (key == "yoffset")
                {
                    converter >> out.chars[id].yoffset;
                }
                else if (key == "xadvance")
                {
                    converter >> out.chars[id].xadvance;
                }
                else if (key == "page")
                {
                    converter >> out.chars[id].page;
                }
                else if (key == "chnl")
                {
                    converter >> out.chars[id].chnl;
                }
            }

        }

    }
}

using namespace std;

const char* vertex_shader =
"#version 400\n"
"layout (location = 0) in vec3 vp;\n"
"layout (location = 1) in vec2 tc;\n"
//"in vec3 vp;"
//"in vec2 vertTexCoord;"
"out vec2 fragTexCoord;\n"
"void main() {\n"
"  gl_Position = vec4(vp, 1.0);\n"
"  fragTexCoord = tc;\n"
"}";

const char* fragment_shader =
"#version 400\n"
"uniform sampler2D tex;\n"
"in vec2 fragTexCoord;\n"
"out vec4 fragColor;\n"
"void main() {\n"
//"fragColor = vec4(fragTexCoord.x, fragTexCoord.y, 0.0, 1.0);"
"fragColor = texture (tex, vec2(fragTexCoord.x, fragTexCoord.y));\n"
//"fragColor = texture(tex, vec2(0.0f, 0.0f));\n"
//"  fragColor = vec4(0.5, 0.0, 0.5, 1.0);"

"}";

//extern const unsigned int SCREEN_WIDTH;
//extern const unsigned int SCREEN_HEIGHT;

extern const int screenPixelWidth;
extern const int screenPixelHeight;
//const float ratio = static_cast<float>(SCREEN_HEIGHT) / SCREEN_WIDTH;

const float ratio = static_cast<float>(screenPixelHeight) / screenPixelWidth;

//glm::vec3 medPoints[] = {{-0.05f, -0.05f, 0.0f},
//                         {-0.05f, 0.05f, 0.0f},
//                         {0.05f, 0.05f, 0.0f},
//                         {0.05f, 0.05f, 0.0f},
//                         {0.05f, -0.05f, 0.0f},
//                         {-0.05f, -0.05f, 0.0f}};

//glm::vec3 medPoints[] = {{-1.0f, 1.0 - (0.03 * ratio), 0.0f},
//                         {-1.0f, 1.0f, 0.0f},
//                         {-0.97f, 1.0f, 0.0f},
//                         {-0.97f, 1.0f, 0.0f},
//                         {-0.97f, 1.0 - (0.03 * ratio), 0.0f},
//                         {-1.0f, 1.0 - (0.03 * ratio), 0.0f}};

// test vtx buffer
//float points[] =
//{
//    -0.05f, -0.05f,  0.0f,
//    -0.05f,  0.05f,  0.0f,
//    0.05f,  0.05f,  0.0f,
//    0.05f,  0.05f,  0.0f,
//    0.05f, -0.05f,  0.0f,
//    -0.05f, -0.05f,  0.0f,
//};

//float texCoords[] =
//{
//    0.0, 0.0,
//    0.0, 1.0,
//    1.0, 1.0,
//    1.0, 1.0,
//    1.0, 0.0,
//    0.0, 0.0,
//};

//float texCoords[] =
//{
//    0.85f, 1.0 - 0.74f,
//    0.85f, 1.0 - 0.68f,
//    0.90f, 1.0 - 0.68f,
//    0.90f, 1.0 - 0.68f,
//    0.90f, 1.0 - 0.74f,
//    0.85f, 1.0 - 0.74f,
//};

FontRenderer::FontRenderer(std::string fileName)
    : fontShader("font")
{
    string fullPath = FONT_LOCATION + fileName;
    parseFontFile(fullPath, fontInfo);

    TextureBroker* pTb = TextureBroker::instance();
    pTb->addTexture("../assets/textures/font/ubuntu_mono_regular_0.png");

#if 0
    RenderBuffers renderBuffers;
    renderBuffers.
#endif
    
    // create vertex buffer
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * bufferSize * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &texCoordsVbo);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordsVbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * bufferSize * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordsVbo);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#if 0
    // compile shaders
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    //glShaderSource(vs, 1, &vertex_shader, NULL);
    shaderSourceFromFile(vs, "font.vs");
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    //glShaderSource(fs, 1, &fragment_shader, NULL);
    shaderSourceFromFile(fs, "font.ps");
    glCompileShader(fs);

    shader = glCreateProgram();
    glAttachShader(shader, fs);
    glAttachShader(shader, vs);
    glLinkProgram(shader);

    GLint isLinked = 0;
    glGetProgramiv(shader, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
        for (int i = 0; i < infoLog.size(); i++) cout << infoLog[i];
        cout << endl;

        // The program is useless now. So delete it.
        glDeleteProgram(shader);

        return;
    }
#endif
}

void FontRenderer::DrawFont(int x, int y, std::string& s)
{
    float cursorAdvance = 0;
    //float cursorAdvanceVal = 0.03f * ratio + .01;
    float scale = 500;

    // convert to ndc
    x -= 1;
    y += 1;


    vec3 vb[256];
    vec2 tcb[256];

    vec3* pvb = vb;
    vec2* ptcb = tcb;

    for (int i = 0; i < s.size(); i++)
    {
        uint32_t asciiVal = s[i];

        // generate character position 
        float xoffset = fontInfo.chars[asciiVal].xoffset / scale;
        float yoffset = fontInfo.chars[asciiVal].yoffset / scale;
        float width = fontInfo.chars[asciiVal].width / scale;
        float height = fontInfo.chars[asciiVal].height / scale;
        float advance = fontInfo.chars[asciiVal].xadvance / scale;

        vec3 topLeftPos = { x + xoffset, y - yoffset, 0 };
        vec3 topRightPos = { x + xoffset + width, y - yoffset, 0 };
        vec3 bottomLeftPos = { x + xoffset, y - yoffset - height, 0 };
        vec3 bottomRightPos = { x + xoffset + width, y - yoffset - height, 0 };

        vec3 cursorPosition[] = { bottomLeftPos, topLeftPos, topRightPos, topRightPos, bottomRightPos, bottomLeftPos};

        cursorPosition[0].x += cursorAdvance;
        cursorPosition[1].x += cursorAdvance;
        cursorPosition[2].x += cursorAdvance;
        cursorPosition[3].x += cursorAdvance;
        cursorPosition[4].x += cursorAdvance;
        cursorPosition[5].x += cursorAdvance;

        // adjust slightly
        //float rectWidth = abs(cursorPosition[1].x - cursorPosition[2].x);
        //float xoff = fontInfo.chars[asciiVal].xoffset * rectWidth / fontInfo.chars[asciiVal].xadvance;
        //
        //float rectHeight = abs(cursorPosition[1].y - cursorPosition[0].y);
        //float yoff = fontInfo.chars[asciiVal].yoffset * rectWidth / fontInfo.chars[asciiVal].xadvance;

        //float charWidth = fontInfo.chars[asciiVal].width * rectWidth / fontInfo.chars[asciiVal].xadvance;
        //float charHeight = fontInfo.chars[asciiVal].height * rectHeight / fontInfo.chars[asciiVal].xadvance;

        //cursorPosition[0].x += xoff;
        //cursorPosition[1].x += xoff;
        //cursorPosition[2].x += xoff;


        // generate text coordinates
        vec2 bottomLeft = { fontInfo.chars[asciiVal].x / 256.0f, 1.0f - (fontInfo.chars[asciiVal].y + fontInfo.chars[asciiVal].height) / 256.0f };
        vec2 topRight = { (fontInfo.chars[asciiVal].x + fontInfo.chars[asciiVal].width) / 256.0f, 1.0f - fontInfo.chars[asciiVal].y / 256.0f };
        vec2 topLeft = { fontInfo.chars[asciiVal].x / 256.0f, 1.0f - (fontInfo.chars[asciiVal].y) / 256.0f };
        vec2 bottomRight = { (fontInfo.chars[asciiVal].x + fontInfo.chars[asciiVal].width) / 256.0f, 1.0f - (fontInfo.chars[asciiVal].y + fontInfo.chars[asciiVal].height) / 256.0f };

        vec2 texCoords[] = { bottomLeft, topLeft, topRight, topRight, bottomRight, bottomLeft };

        // copy quad
        memcpy_s(pvb, sizeof(cursorPosition), cursorPosition, sizeof(cursorPosition));

        memcpy_s(ptcb, sizeof(texCoords), texCoords, sizeof(texCoords));
        size_t increment = sizeof(cursorPosition);
        pvb += 6;
        ptcb += 6;

        cursorAdvance += advance - xoffset;
    }

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glCheck();

    float* pData = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    glCheck();

    if (pData)
    {
        memcpy_s(pData, 18 * sizeof(float) * s.size(), vb, 18 * sizeof(float) * s.size());
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glCheck();
    }

    glBindBuffer(GL_ARRAY_BUFFER, texCoordsVbo);
    glCheck();
    pData = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    if (pData)
    {
        memcpy_s(pData, 12 * sizeof(float) * s.size(), tcb, 12 * sizeof(float) * s.size());
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glCheck();

    fontShader.runShader(vao, s);
#if 0
    glUseProgram(shader);

    GLuint texLoc = glGetUniformLocation(shader, "tex");
    glUniform1i(texLoc, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontTex);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6 * static_cast<GLsizei>(s.size()));
#endif
}

