#ifndef MINECRAFT_CLONE_VBO_H
#define MINECRAFT_CLONE_VBO_H

#include "core.h"

class VBO {
    public:
        GLuint ID;
        VBO(const GLfloat *vertices, const GLsizeiptr size);
        void bind() const;
        void unbind() const;
        static void free(VBO *vbo);
};

#endif
