#include "core/EBO.h"

EBO::EBO(GLuint *indices, GLsizeiptr size) {
    glGenBuffers(1, &ID);
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

void EBO::bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void EBO::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::free(EBO *ebo) {
    if(ebo != nullptr) {
        glDeleteBuffers(1, &ebo->ID);
        delete ebo;
    }
}
