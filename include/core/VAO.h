#ifndef MINECRAFT_CLONE_VAO_H
#define MINECRAFT_CLONE_VAO_H

#include "core.h"
#include "core/VBO.h"

class VAO {
    public:
        GLuint ID;
        
        VAO();
        
        void linkAttrib(VBO *vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void *offset);
        void bind();
        void unbind();
        static void free(VAO *vao);
};

#endif
