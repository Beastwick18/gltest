#ifndef MINECRAFT_CLONE_VBLAYOUT_H
#define MINECRAFT_CLONE_VBLAYOUT_H

#include <cassert>
#include "core.h"

typedef struct {
    GLuint type;
    unsigned int count;
    unsigned char normalized;
    static unsigned int getSize(GLenum type) {
        switch (type) {
            case GL_FLOAT:          return 4;
            case GL_UNSIGNED_INT:   return 4;
            case GL_UNSIGNED_BYTE:  return 1;
        }
        fprintf(stderr, "Non fatal: Unrecognized type [in file %s line %d]\n", __FILE__, __LINE__);
        return 0;
    }
} VBelement;

class VBlayout {
    private:
        unsigned int stride = 0;
        std::vector<VBelement> elements;
    public:
        template<typename T>
        void push(unsigned int count);
        
        inline std::vector<VBelement> getElements() const { return elements; }
        inline unsigned int getStride() const { return stride; }
};

#endif
