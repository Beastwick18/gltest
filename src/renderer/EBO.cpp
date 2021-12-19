#include "renderer/EBO.h"

EBO::EBO(const GLuint *indices, const GLuint count) {
    this->count = count;
    glGenBuffers(1, &ID);
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), indices, GL_STATIC_DRAW);
}

void EBO::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void EBO::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::free(EBO *ebo) {
    if(ebo != nullptr) {
        glDeleteBuffers(1, &ebo->ID);
        delete ebo;
    }
}
