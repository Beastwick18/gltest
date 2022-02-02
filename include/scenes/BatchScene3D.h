#ifndef BATCH_SCENE_3D_H
#define BATCH_SCENE_3D_H

#include "renderer/cubemap.h"
#include "renderer/spriteSheet.h"
#include "scenes/scene.h"
#include "core/window.h"
#include "renderer/Batch.hpp"
#include "renderer/texture2D.h"
#include "renderer/Frustum.h"
#include <mutex>
#include <future>
#include "world/Chunk.h"
#include "world/World.h"
#include <functional>

class BatchScene3D : public Scene {
    public:
        BatchScene3D(MinecraftClone::Window *window);
        ~BatchScene3D() override;
        
        void render() override;
        void guiRender() override;
        void update(double deltaTime) override;
        
    private:
        MinecraftClone::Window *window;
        Camera *c;
        Frustum *f;
        GLint mView, mProj, mTime;
        RaycastResults ray;
        glm::mat4 blockView;
        float guiScale;
        double dtSum;
        Mesh<Vertex> highlightMesh;
        Mesh<Vertex> invMesh;
        
        int blockInHand;
        const int invSize = 12;
        BlockID inv[12];
        
        std::vector<std::future<void>> meshFutures;
        bool wiremeshToggle = false, wiremesh = false, showGui = false;
};

#endif
