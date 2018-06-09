/*
* ModelFactory is part of the ReBoot distribution (https://github.com/octopusprime314/ReBoot.git).
* Copyright (c) 2017 John Lukasiewicz.
*
* ReBoot is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 3.
*
* ReBoot is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/**
*  This class contains code to load in a font and draw it in screen space.
*/

#pragma once
#include <cstdlib>
#include <string>
#include "GLIncludes.h"
#include "FontShader.h"

const std::string FONT_LOCATION = "../assets/textures/font/";

struct charInfo
{
    int32_t id;       // The character id.
    int32_t x;        // The left position of the character image in the texture.
    int32_t y;        // The top position of the character image in the texture.
    int32_t width;    // The width of the character image in the texture.
    int32_t height;   // The height of the character image in the texture.
    int32_t xoffset;  // How much the current position should be offset when copying the image from the texture to the screen.
    int32_t yoffset;  // How much the current position should be offset when copying the image from the texture to the screen.
    int32_t xadvance; // How much the current position should be advanced after drawing the character.
    int32_t page;     // The texture page where the character image is found.
    int32_t chnl;     // The texture channel where the character image is found(1 = blue, 2 = green, 4 = red, 8 = alpha, 15 = all channels).
};

struct FontInfo
{
#if 0
    // info
    char face[256];        // This is the name of the true type font.
    uint32_t size;         // The size of the true type font.
    bool bold;             // The font is bold.
    bool italic;           // The font is italic.
    char charset[256];     // The name of the OEM charset used (when not unicode).
    bool unicode;          // Set to 1 if it is the unicode charset.
    uint32_t stretchH;     // The font height stretch in percentage. 100 % means no stretch.
    uint32_t smooth;       // Set to 1 if smoothing was turned on.
    uint32_t aa;           // The supersampling level used. 1 means no supersampling was used.
    uint32_t paddingUp;    // The padding for each character (up, right, down, left).
    uint32_t paddingDown;
    uint32_t paddingLeft;
    uint32_t paddingRight;
    uint32_t spacingHoriz; // The spacing for each character (horizontal, vertical).
    uint32_t spacingVert;
    uint32_t outline;      // The outline thickness for the characters.

                           // common
    uint32_t lineHeight; // This is the distance in pixels between each line of text.
    uint32_t base;       // The number of pixels from the absolute top of the line to the base of the characters.
    uint32_t scaleW;     // The width of the texture, normally used to scale the x pos of the character image.
    uint32_t scaleH;     // The height of the texture, normally used to scale the y pos of the character image.
    uint32_t pages;      // The number of texture pages included in the font.
    uint32_t packed;     // Set to 1 if the monochrome characters have been packed into each of the texture channels.In this case alphaChnl describes what is stored in each channel.
    uint32_t alphaChn;   // Set to 0 if the channel holds the glyph data, 1 if it holds the outline, 2 if it holds the glyph and the outline, 3 if its set to zero, and 4 if its set to one.
    uint32_t redChnl;    // Set to 0 if the channel holds the glyph data, 1 if it holds the outline, 2 if it holds the glyph and the outline, 3 if its set to zero, and 4 if its set to one.
    uint32_t greenChn;   // Set to 0 if the channel holds the glyph data, 1 if it holds the outline, 2 if it holds the glyph and the outline, 3 if its set to zero, and 4 if its set to one.
    uint32_t blueChnl;   // Set to 0 if the channel holds the glyph data, 1 if it holds the outline, 2 if it holds the glyph and the outline, 3 if its set to zero, and 4 if its set to one.

                         // page
    uint32_t id;     //The page id.
    char file[256];  //The texture file name.
#endif 
    uint32_t charCount;

    charInfo chars[256];
};

void parseFontFile(std::string& filename, FontInfo& out);

class FontRenderer
{
public:
    FontRenderer() = delete;
    FontRenderer(std::string fileName);
    void DrawFont(int x, int y, std::string& s);
    GLuint getTexture() { return fontTex; }
    GLuint getVao() { return vao; }
    ~FontRenderer() {};
private:
    const int bufferSize = 256;
    FontInfo fontInfo;
    FontShader fontShader;

    GLuint fontTex;
    GLuint vao;
    GLuint texCoordsVbo;
    GLuint vbo;
    GLuint shader;

};