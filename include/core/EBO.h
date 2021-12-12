#ifndef MINECRAFT_CLONE_EBO_H
#define MINECRAFT_CLONE_EBO_H

#include "core.h"

class EBO {
    public:
        GLuint ID;
        
        EBO(GLuint *indices, GLsizeiptr size);
        
        void bind();
        void unbind();
        static void free(EBO *ebo);
};

#endif
