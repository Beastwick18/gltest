#include "core/VAO.h"

VAO::VAO() {
    glGenVertexArrays(1, &ID);
}

void VAO::linkAttrib(const VBO *vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void *offset) {
    vbo->bind();
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
    glEnableVertexAttribArray(layout);
    vbo->unbind();
}

void VAO::addBuffer(const VBO *vbo, const VBlayout *layout) {
    vbo->bind();
    int i = 0;
    unsigned int offset = 0;
    for(const auto &e : layout->getElements()) {
        printf("%d, %d, %d, %d\n", i, e.count, layout->getStride(), offset);
        glVertexAttribPointer(i, e.count, e.type, e.normalized, layout->getStride(), reinterpret_cast<void *>(offset));
        offset += e.count * VBelement::getSize(e.type);
        glEnableVertexAttribArray(i++);
    }
    vbo->unbind();
}

// template<typename T>
// void VAO::pushAttrib(VBO *vbo, GLuint count, GLuint vertexPropCount) {
    
// }

// template<>
// void VAO::pushAttrib<float>(VBO *vbo, GLuint count, GLuint vertexPropCount) {
//     linkAttrib(vbo, layoutCount++, count, GL_FLOAT, vertexPropCount * sizeof(float), (void *)(attribEnd*sizeof(float)));
//     attribEnd += count;
// }

void VAO::bind() const {
    glBindVertexArray(ID);
}

void VAO::unbind() const {
    glBindVertexArray(0);
}

void VAO::free(VAO *vao) {
    if(vao != nullptr) {
        glDeleteVertexArrays(1, &vao->ID);
        delete vao;
    }
}
