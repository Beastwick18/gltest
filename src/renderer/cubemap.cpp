#include "renderer/cubemap.h"
#include "stb/stb_image.h"

CubeMap::CubeMap() {
    
}

CubeMap *CubeMap::loadFromImageFile(const std::vector<std::string> imagePaths, const GLenum internalFormat, const GLenum filter, const GLenum repeat) {
    CubeMap *m = new CubeMap;
    glGenTextures(1, &m->ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m->ID);
    
    stbi_set_flip_vertically_on_load(0);
    int width, height, imageChannels;
    for(unsigned int i = 0; i < imagePaths.size(); i++) {
        unsigned char *data = stbi_load(imagePaths[i].c_str(), &width, &height, &imageChannels, 0);
        GLenum format;
        if(imageChannels == 3)
            format = GL_RGB;
        else if(imageChannels == 4)
            format = GL_RGBA;
        else {
            fprintf(stderr, "Non fatal: Invalid image format [in file %s line %d]\n", __FILE__, __LINE__);
            return nullptr;
        }
        if(data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            fprintf(stderr, "Failed to load cubemap\n");
            return nullptr;
        }
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, repeat);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, repeat);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, repeat);
    
    return m;
}

void CubeMap::bind() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}

void CubeMap::unbind() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
