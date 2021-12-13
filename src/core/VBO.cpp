#include "core/VBO.h"

VBO::VBO(const GLfloat *vertices, const GLsizeiptr size) {
    glGenBuffers(1, &ID);
    bind();
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void VBO::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::free(VBO *vbo) {
    if(vbo != nullptr) {
        glDeleteBuffers(1, &vbo->ID);
        delete vbo;
    }
}
