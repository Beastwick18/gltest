#ifndef MINECRAFT_CLONE_VAO_H
#define MINECRAFT_CLONE_VAO_H

#include "core.h"
#include "core/VBO.h"
#include "core/VBlayout.h"

class VAO {
    public:
        GLuint ID;
        GLuint attribEnd, layoutCount;
        
        VAO();
        
        void linkAttrib(const VBO *vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void *offset);
        void addBuffer(const VBO *vbo, const VBlayout *layout);
        
        // template<typename T>
        // void pushAttrib(VBO *vbo, GLuint count, GLuint vertexPropCount);
        
        void bind() const;
        void unbind() const;
        static void free(VAO *vao);
};

#endif
