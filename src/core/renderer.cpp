#include "core/renderer.h"

void Renderer::render(const VAO *vao, const EBO *ebo, const Shader *s) const {
    s->use();
    vao->bind();
    ebo->bind();
    glDrawElements(GL_TRIANGLES, ebo->getCount(), GL_UNSIGNED_INT, 0);
}
