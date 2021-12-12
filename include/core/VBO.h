#ifndef MINECRAFT_CLONE_VBO_H
#define MINECRAFT_CLONE_VBO_H

#include "core.h"

class VBO {
    public:
        GLuint ID;
        VBO(GLfloat *vertices, GLsizeiptr size);
        void bind();
        void unbind();
        static void free(VBO *vbo);
};

#endif
