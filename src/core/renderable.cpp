#include "core/renderable.h"

Renderable::Renderable(GLfloat *vertices, GLuint vertexPropertyCount, GLuint vertexCount, GLuint *indices, GLuint indicesCount) {
    this->vertexPropertyCount = vertexPropertyCount;
    this->vertexCount = vertexCount;
    this->indicesCount = indicesCount;
    vao = new VAO;
    vao->bind();
    vbo = new VBO(vertices, sizeof(GLfloat) * vertexCount * vertexPropertyCount);
    ebo = new EBO(indices, indicesCount);
}

void Renderable::unbindAll() const {
    vbo->unbind();
    ebo->unbind();
    vao->unbind();
}

template <typename T>
void Renderable::pushAttrib(GLuint count) {
    
}

template <>
void Renderable::pushAttrib<float>(GLuint count) {
    vao->bind();
    // vao->pushAttrib<float>(vbo, count, vertexPropertyCount);
    
    vao->unbind();
}

void Renderable::free(Renderable *r) {
    if(r != nullptr) {
        VAO::free(r->vao);
        VBO::free(r->vbo);
        EBO::free(r->ebo);
        delete r;
    }
}

void Renderable::render() {
    vao->bind();
    glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
    vao->unbind();
}

void Renderable::render(Texture2D *tex) {
    tex->bind();
    render();
}
