#ifndef MINECRAFT_CLONE_VBO_H
#define MINECRAFT_CLONE_VBO_H

#include "core.h"

class VBO {
    public:
        GLuint ID;
        size_t size;
        VBO(const GLfloat *vertices, const GLsizeiptr size, const GLenum drawType);
        VBO(const GLsizeiptr size);
        
        void bind() const;
        void unbind() const;
        void setData(const void *data, const GLsizeiptr size) const;
        
        static void free(VBO *vbo);
};

#endif
