#ifndef MINECRAFT_CLONE_RENDERER_H
#define MINECRAFT_CLONE_RENDERER_H

#include "renderer/EBO.h"
#include "renderer/VAO.h"
#include "renderer/VBO.h"
#include "renderer/shader.h"

class Renderer {
    public:
        void render(const VAO *vao, const EBO *ebo, const Shader *s) const;
};

#endif
