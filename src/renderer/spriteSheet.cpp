#include "renderer/spriteSheet.h"
#include "stb/stb_image.h"

SpriteSheet::SpriteSheet(const unsigned char *bytes, unsigned int imageWidth, unsigned int imageHeight, int rows, int cols, const GLenum format,
                     const GLenum internalFormat, const GLenum filter, const GLenum repeat) : Texture2D(bytes, imageWidth, imageHeight, format, internalFormat, filter, repeat) {
    this->rows = rows;
    this->cols = cols;
}

SpriteSheet *SpriteSheet::loadFromImageFile(const char *imagePath, int rows, int cols, const GLenum internalFormat, const GLenum filter, const GLenum repeat) {
    int imageWidth, imageHeight, imageChannels;
    GLenum format = 0;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *bytes = stbi_load(imagePath, &imageWidth, &imageHeight, &imageChannels, 0);
    if(imageChannels == 3)
        format = GL_RGB;
    else if(imageChannels == 4)
        format = GL_RGBA;
    else {
        fprintf(stderr, "Non fatal: Invalid image format [in file %s line %d]\n", __FILE__, __LINE__);
        return nullptr;
    }
    SpriteSheet *tex = new SpriteSheet(bytes, imageWidth, imageHeight, rows, cols, format, internalFormat, filter, repeat);
    stbi_image_free(bytes);
    return tex;
}

TexCoords SpriteSheet::getSubTexture(int x, int y, int w, int h) const {
    return { (float)(x)/cols, (float)(y)/rows, (float)(w)/cols, (float)(h)/rows };
}
