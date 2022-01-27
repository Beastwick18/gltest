#include "renderer/texture2D.h"
#include "stb/stb_image.h"

Texture2D::Texture2D() {};

Texture2D::Texture2D(const unsigned char *bytes, unsigned int imageWidth, unsigned int imageHeight, const GLenum format,
                     const GLenum internalFormat, const GLenum filter, const GLenum repeat) {
    this->imageWidth = imageWidth;
    this->imageHeight = imageHeight;
    glCreateTextures(GL_TEXTURE_2D, 1, &ID);
    
    glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, filter);
    glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, filter);
    
    glTextureParameteri(ID, GL_TEXTURE_WRAP_S, repeat);
    glTextureParameteri(ID, GL_TEXTURE_WRAP_T, repeat);
    
    glTextureStorage2D(ID, 1, internalFormat, imageWidth, imageHeight);
    glTextureSubImage2D(ID, 0, 0, 0, imageWidth, imageHeight, format, GL_UNSIGNED_BYTE, bytes);
    glGenerateTextureMipmap(ID);
}

Texture2D *Texture2D::loadFromImageFile(const char *imagePath, const GLenum internalFormat,
                                        const GLenum filter, const GLenum repeat) {
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
    Texture2D *tex = new Texture2D(bytes, imageWidth, imageHeight, format, internalFormat, filter, repeat);
    stbi_image_free(bytes);
    return tex;
}

void Texture2D::bind(const unsigned int index) const {
    glBindTextureUnit(index, ID);
}

void Texture2D::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::free(Texture2D *tex) {
    if(tex != nullptr) {
        glDeleteTextures(1, &tex->ID);
        delete tex;
    }
}
