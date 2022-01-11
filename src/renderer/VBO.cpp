#include "renderer/VBO.h"

VBO::VBO(const GLfloat *vertices, const GLsizeiptr size, const GLenum drawType) : size(size){
    glCreateBuffers(1, &ID);
    glNamedBufferData(ID, size, vertices, drawType);
}

VBO::VBO(const GLsizeiptr size) : size(size) {
    glCreateBuffers(1, &ID);
    glNamedBufferData(ID, size, nullptr, GL_DYNAMIC_DRAW);
}

void VBO::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::setData(const void *data, const GLsizeiptr size) const {
    glNamedBufferData(ID, size, data, GL_DYNAMIC_DRAW);
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
