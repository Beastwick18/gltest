#ifndef MINECRAFT_CLONE_RENDERER_H
#define MINECRAFT_CLONE_RENDERER_H

#include "renderer/Batch.hpp"
#include "renderer/camera.h"
#include "renderer/shader.h"
#include "world/Block.h"

namespace Renderer {
    extern Shader *regularShader, *transparentShader;
    extern GLint vpUniform, wVpUniform;
    extern GLint waveUniform;
    extern GLint sunUniform, wSunUniform;
    extern Batch<Vertex> regularBatch;
    extern Batch<Vertex> transparentBatch;
    extern float skyBrightness;
    
    void init();
    void free();
    void setCamera(const Camera *cameraPtr);
    void generateQuadMesh(Mesh<Vertex> &newMesh, Vertex v0, Vertex v1, Vertex v2, Vertex v3);
    void generateCubeMesh(Mesh<Vertex> &newMesh, float x, float y, float z, BlockTexture tex, bool top, bool bottom, bool left, bool right, bool front, bool back, float light = 1.f, float skyLight = 1.f);
    void renderMesh(const Vertex *mesh, const size_t size);
    void renderTransparentMesh(const Vertex *mesh, const size_t size);
    void render();
    void flushRegularBatch();
    void flushTransparentBatch();
    void update(double deltaTime);
}

#endif
