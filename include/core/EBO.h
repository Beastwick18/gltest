#ifndef MINECRAFT_CLONE_EBO_H
#define MINECRAFT_CLONE_EBO_H

#include "core.h"

class EBO {
    public:
        GLuint ID;
        
        EBO(const GLuint *indices, const GLuint count);
        
        void bind() const;
        void unbind() const;
        static void free(EBO *ebo);
};

#endif
