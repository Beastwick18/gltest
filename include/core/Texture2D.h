#ifndef MINECRAFT_CLONE_TEXTURE2D_H
#define MINECRAFT_CLONE_TEXTURE2D_H

#include "core.h"

class Texture2D {
    public:
        GLuint ID;
        GLenum textureIndex;
        Texture2D(const GLenum texture, const unsigned char *bytes, unsigned int imageWidth, unsigned int imageHeight, const GLenum format, const GLenum internalFormat, const GLenum filter, const GLenum repeat);
        static Texture2D *loadFromImageFile(std::string imagePath, const GLuint texture = 0, const GLenum internalFormat = GL_RGBA, const GLenum filter = GL_NEAREST, const GLenum repeat = GL_REPEAT);
        void bind() const;
        void unbind() const;
        void activate() const;
        static void free(Texture2D *tex);
};

#endif
