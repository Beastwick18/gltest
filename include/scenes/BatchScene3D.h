#ifndef BATCH_SCENE_3D_H
#define BATCH_SCENE_3D_H

#include "scenes/scene.h"
#include "core/window.h"
#include "renderer/Batch.hpp"
#include "renderer/texture2D.h"
#include "renderer/Frustum.h"
#include <mutex>
#include <future>

struct Quad {
    Vertex vertices[6];
};

struct Block {
    glm::vec3 position;
    Quad faces[6];
};

struct ChunkMesh {
    int chunkX, chunkZ;
    // Vertex *v;
    std::vector<Vertex> v;
    // int size;
};

class BatchScene3D : public Scene {
    public:
        BatchScene3D(MinecraftClone::Window *window);
        ~BatchScene3D() override;
        static void createAllVertices(std::vector<ChunkMesh> *meshes, int startx, int startz, int chunkW, int chunkL);
        static Quad generateQuad(Vertex v0, Vertex v1, Vertex v2, Vertex v3);
        static void generateCube(std::vector<Quad>& quads, float x, float y, float z, TexCoords topTex, TexCoords bottomTex, TexCoords sideTex, bool top, bool bottom, bool left, bool right, bool front, bool back);
        static void generateMesh(std::vector<Quad> quads);
        
        static float getNoise(float x, float z);
        bool drawVertexArray(const Vertex *array, const int size);
        bool drawCube(glm::vec3 position, glm::vec2 texCoords);
        
        void render(const Renderer &r) override;
        void guiRender() override;
        void update(double deltaTime) override;
        
    private:
        MinecraftClone::Window *window;
        std::vector<Batch<Vertex>> batches;
        Texture2D *block_atlas;
        Shader *s;
        Camera *c;
        Frustum *f;
        VBlayout layout;
        
        // std::vector<Quad> quads;
        // Vertex *mesh;
        std::vector<ChunkMesh> meshes;
        std::vector<std::future<void>> meshFutures;
        bool wiremeshToggle = false, wiremesh = false;
        
        GLint vpUniform;
        GLint modelUniform;
};

#endif
