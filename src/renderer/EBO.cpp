#include "renderer/EBO.h"

EBO::EBO(const GLuint *indices, const GLuint count) {
    this->count = count;
    glCreateBuffers(1, &ID);
    glNamedBufferData(ID, count * sizeof(GLuint), indices, GL_STATIC_DRAW);
}

EBO::EBO(const GLuint count) {
    this->count = count;
    glCreateBuffers(1, &ID);
    glNamedBufferData(ID, count * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
}

void EBO::setData(const void *data, const GLsizeiptr size) {
    glNamedBufferData(ID, size, data, GL_DYNAMIC_DRAW);
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
