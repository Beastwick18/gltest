#ifndef MINECRAFT_CLONE_CUBEMAP_H
#define MINECRAFT_CLONE_CUBEMAP_H

#include "renderer/texture2D.h"

struct CubeMapBytes {
    unsigned char *topBytes;
    unsigned char *bottomBytes;
    unsigned char *leftBytes;
    unsigned char *rightBytes;
    unsigned char *frontBytes;
    unsigned char *backBytes;
};

class CubeMap: public Texture2D {
    public:
        // CubeMap(const CubeMapBytes &bytes, unsigned int imageWidth, unsigned int imageHeight, const GLenum format, const GLenum internalFormat, const GLenum filter, const GLenum repeat);
        CubeMap();
        static CubeMap *loadFromImageFile(const std::vector<std::string> imagePaths, const GLenum internalFormat = GL_RGBA8, const GLenum filter = GL_NEAREST, const GLenum repeat = GL_CLAMP_TO_EDGE);
        void bind() const;
        void unbind() const;
};

#endif
