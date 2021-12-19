#ifndef BATCH_SCENE_3D_H
#define BATCH_SCENE_3D_H

#include "scenes/scene.h"
#include "core/window.h"
#include "renderer/Batch.hpp"
#include "renderer/texture2D.h"

struct Quad {
    Vertex vertices[6];
};

class BatchScene3D : public Scene {
    public:
        BatchScene3D(MinecraftClone::Window *window);
        ~BatchScene3D() override;
        void createAllVertices();
        
        bool drawVertexArray(Vertex *array, const int size);
        
        void render(const Renderer &r) override;
        void guiRender() override;
        void update(double deltaTime) override;
    private:
        MinecraftClone::Window *window;
        std::vector<Batch<Vertex>> batches;
        Texture2D *block_atlas;
        Shader *s;
        Camera *c;
        VBlayout layout;
        
        std::vector<Quad> quads;
        
        GLint vpUniform;
        GLint modelUniform;
};

#endif
