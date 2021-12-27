#ifndef MINECRAFT_CLONE_RENDERER_H
#define MINECRAFT_CLONE_RENDERER_H

#include "renderer/EBO.h"
#include "renderer/VAO.h"
#include "renderer/VBO.h"
#include "renderer/shader.h"
#include "renderer/Batch.hpp"

struct Quad {
    Vertex vertices[6];
};

namespace Renderer {
        
        void init();
        void free();
        void setCamera(const Camera *cameraPtr);
        void renderMesh(const Vertex *mesh, const size_t size);
        void renderTransparentMesh(const Vertex *mesh, const size_t size);
        void render();
        void update(double deltaTime);
        
}

#endif
