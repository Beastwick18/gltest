#ifndef MINECRAFT_CLONE_RENDERABLE_H
#define MINECRAFT_CLONE_RENDERABLE_H

#include "core/VBO.h"
#include "core/EBO.h"
#include "core/VAO.h"
#include "core/texture2D.h"

struct Renderable {
    VBO *vbo;
    EBO *ebo;
    VAO *vao;
    GLuint vertexPropertyCount, vertexCount, indicesCount;
    
    Renderable(GLfloat *vertices, GLuint vertexPropertyCount, GLuint vertexCount, GLuint *indices, GLuint indicesCount);
    
    void unbindAll() const;
    
    template <typename T>
    void pushAttrib(GLuint count);
    
    static void free(Renderable *r);
    
    void render();
    void render(Texture2D *tex);
};

#endif
