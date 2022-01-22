#ifndef MINECRAFT_CLONE_RENDERER_H
#define MINECRAFT_CLONE_RENDERER_H

#include "renderer/Batch.hpp"
#include "renderer/camera.h"
#include "world/Block.h"

namespace Renderer {
    void init();
    void free();
    void setCamera(const Camera *cameraPtr);
    void generateQuadMesh(std::vector<Vertex> &newMesh, Vertex v0, Vertex v1, Vertex v2, Vertex v3);
    void generateCubeMesh(std::vector<Vertex> &newMesh, float x, float y, float z, BlockTexture tex, bool top, bool bottom, bool left, bool right, bool front, bool back);
    void renderMesh(const Vertex *mesh, const size_t size);
    void renderTransparentMesh(const Vertex *mesh, const size_t size);
    void render();
    void flushRegularBatch();
    void flushTransparentBatch();
    void update(double deltaTime);
}

#endif
