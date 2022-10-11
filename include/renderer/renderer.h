#ifndef MINECRAFT_CLONE_RENDERER_H
#define MINECRAFT_CLONE_RENDERER_H

#include "renderer/Batch.hpp"
#include "renderer/camera.h"
#include "world/World.h"
#include "renderer/shader.h"
#include "world/Block.h"

namespace Renderer {
    extern Shader *regularShader, *transparentShader;
    extern GLint viewUniform, wViewUniform, projUniform, wProjUniform;
    extern GLint texUniform, wTexUniform;
    extern int frame;
    extern GLint waveUniform;
    extern GLint sunUniform, wSunUniform;
    extern Batch<Vertex> regularBatch;
    extern Batch<Vertex> transparentBatch;
    extern float skyBrightness, wave;
    extern const Camera *camera;
    extern const MinecraftClone::Window *window;
    
    void init(MinecraftClone::Window *window);
    void free();
    void setCamera(const Camera *cameraPtr);
    void generateQuadMesh(Mesh<Vertex> &newMesh, Vertex v0, Vertex v1, Vertex v2, Vertex v3);
    void generateCubeMesh(Mesh<Vertex> &mesh, glm::vec3 pos, BlockTexture tex, SurroundingBlocks surr, LightData light = 0xFF);
    void generateCubeMesh(Mesh<Vertex> &mesh, float x, float y, float z, BlockTexture tex, bool top, bool bottom, bool left, bool right, bool front, bool back, LightData light = 0xFF);
    void generateLiquidMesh(Mesh<Vertex> &mesh, glm::vec3 pos, BlockTexture tex, SurroundingBlocks surr, LightData light = 0xFF);
    void generateLiquidMesh(Mesh<Vertex> &mesh, float x, float y, float z, BlockTexture tex, bool top, bool bottom, bool left, bool right, bool front, bool back, LightData light = 0xFF);
    void generateTorchMesh(Mesh<Vertex> &mesh, float x, float y, float z, BlockTexture tex, SurroundingBlocks adj, LightData light = 0xFF);
    void generateCrossMesh(Mesh<Vertex> &mesh, float x, float y, float z, BlockTexture tex, SurroundingBlocks adj, LightData light = 0xFF);
    void renderMesh(const Vertex *mesh, const size_t size, const GLuint *indices, const size_t idc_size);
    void renderTransparentMesh(const Vertex *mesh, const size_t size, const GLuint *indices, const size_t idc_size);
    void render();
    void renderCubemap();
    void renderCrosshair();
    void flushRegularBatch();
    void flushTransparentBatch();
    void update(double deltaTime);
}

#endif
