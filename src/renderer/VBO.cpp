#include "renderer/VBO.h"

VBO::VBO(const GLfloat *vertices, const GLsizeiptr size, const GLenum drawType) : size(size){
    glCreateBuffers(1, &ID);
    bind();
    glBufferData(GL_ARRAY_BUFFER, size, vertices, drawType);
}

VBO::VBO(const GLsizeiptr size) : size(size) {
    glGenBuffers(1, &ID);
    bind();
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

void VBO::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}

// template<typename T>
// void VBO::bind(T *vertices) const {
//     bind();
//     glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
// }

void VBO::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::free(VBO *vbo) {
    if(vbo != nullptr) {
        glDeleteBuffers(1, &vbo->ID);
        delete vbo;
    }
}
