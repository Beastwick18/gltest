#ifndef MINECRAFT_CLONE_TEXTURE2D_H
#define MINECRAFT_CLONE_TEXTURE2D_H

#include "core.h"

class Texture2D {
    public:
        GLuint ID;
        
        Texture2D(const unsigned char *bytes, unsigned int imageWidth, unsigned int imageHeight, const GLenum format, const GLenum internalFormat, const GLenum filter, const GLenum repeat);
        
        static Texture2D *loadFromImageFile(const std::string &imagePath, const GLenum internalFormat = GL_RGBA8, const GLenum filter = GL_NEAREST, const GLenum repeat = GL_CLAMP_TO_EDGE);
        static void free(Texture2D *tex);
        
        void bind(const unsigned int index = 0) const;
        void unbind() const;
};

#endif
