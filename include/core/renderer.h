#ifndef MINECRAFT_CLONE_RENDERER_H
#define MINECRAFT_CLONE_RENDERER_H

#include "core/EBO.h"
#include "core/VAO.h"
#include "core/VBO.h"
#include "core/shader.h"

class Renderer {
    public:
        void render(const VAO *vao, const EBO *ebo, const Shader *s);
};

#endif
