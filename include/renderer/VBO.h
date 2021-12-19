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
        
        // template<typename T>
        // void bind(T *vertices) const;
        
        void unbind() const;
        static void free(VBO *vbo);
};

#endif
