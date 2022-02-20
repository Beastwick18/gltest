#include "renderer/VAO.h"

VAO::VAO() {
    glCreateVertexArrays(1, &ID);
}

void VAO::linkAttrib(const VBO *vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void *offset) {
    glVertexArrayAttribBinding(ID, layout, 0);
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
    glEnableVertexArrayAttrib(ID, layout);
    glVertexArrayVertexBuffer(ID, 0, vbo->ID, 0, stride);
}

void VAO::addBuffer(const VBO *vbo, const VBlayout &layout) {
    unsigned int i = 0, offset = 0;
    for(const auto &e : layout.getElements()) {
        glVertexArrayAttribBinding(ID, i, 0);
        if(e.type == GL_INT) {
            glVertexArrayAttribIFormat(ID, i, e.count, e.type, offset);
        } else
            glVertexArrayAttribFormat(ID, i, e.count, e.type, e.normalized, offset);
        offset += e.count * VBelement::getSize(e.type);
        glEnableVertexArrayAttrib(ID, i++);
    }
    glVertexArrayVertexBuffer(ID, 0, vbo->ID, 0, layout.getStride());
}

void VAO::bind() const {
    glBindVertexArray(ID);
}

void VAO::unbind() const {
    glBindVertexArray(0);
}

void VAO::free(VAO *vao) {
    if(vao == nullptr) return;
    
    glDeleteVertexArrays(1, &vao->ID);
    delete vao;
}
