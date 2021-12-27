#ifndef MINECRAFT_CLONE_SPRITE_SHEET_H
#define MINECRAFT_CLONE_SPRITE_SHEET_H

#include "renderer/texture2D.h"

class SpriteSheet : public Texture2D {
    public:
        SpriteSheet(const unsigned char *bytes, unsigned int imageWidth, unsigned int imageHeight, int rows, int cols, const GLenum format, const GLenum internalFormat, const GLenum filter, const GLenum repeat);
        static SpriteSheet *loadFromImageFile(const char *imagePath, int rows, int cols, const GLenum internalFormat = GL_RGBA8, const GLenum filter = GL_NEAREST, const GLenum repeat = GL_CLAMP_TO_EDGE);
        TexCoords getSubTexture(int x, int y, int w = 1, int h = 1) const;
    private:
        int rows, cols;
};

#endif
