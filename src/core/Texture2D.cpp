#include "core/Texture2D.h"
#include "stb/stb_image.h"

Texture2D::Texture2D(const GLenum texture, const unsigned char *bytes, unsigned int imageWidth, unsigned int imageHeight, const GLenum format,
                     const GLenum internalFormat, const GLenum filter, const GLenum repeat) {
    glGenTextures(1, &ID);
    this->textureIndex = texture;
    glActiveTexture(texture + GL_TEXTURE0);
    bind();
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat);
    
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, imageWidth, imageHeight, 0, format, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    unbind();
}

Texture2D *Texture2D::loadFromImageFile(std::string imagePath, const GLuint texture, const GLenum internalFormat,
                                        const GLenum filter, const GLenum repeat) {
    int imageWidth, imageHeight, imageChannels;
    GLenum format = 0;
    unsigned char *bytes = stbi_load(imagePath.c_str(), &imageWidth, &imageHeight, &imageChannels, 0);
    if(imageChannels == 3)
        format = GL_RGB;
    else if(imageChannels == 4)
        format = GL_RGBA;
    else {
        fprintf(stderr, "Non fatal: Invalid image format [in file %s line %d]\n", __FILE__, __LINE__);
        return nullptr;
    }
    Texture2D *tex = new Texture2D(texture, bytes, imageWidth, imageHeight, format, internalFormat, filter, repeat);
    stbi_image_free(bytes);
    return tex;
}

void Texture2D::bind() const {
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture2D::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::activate() const {
    glActiveTexture(textureIndex + GL_TEXTURE0);
}

void Texture2D::free(Texture2D *tex) {
    if(tex != nullptr) {
        glDeleteTextures(1, &tex->ID);
        delete tex;
    }
}
