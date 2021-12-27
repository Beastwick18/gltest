#ifndef BATCH_RENDERING_SCENE_H
#define BATCH_RENDERING_SCENE_H

#include "scenes/scene.h"
#include "renderer/renderer.h"
#include "core/window.h"
#include "renderer/Batch.hpp"
#include "renderer/texture2D.h"

class BatchRendering2DScene : public Scene {
    public:
        BatchRendering2DScene(MinecraftClone::Window *window);
        ~BatchRendering2DScene() override;
        
        void render() override;
        void guiRender() override;
    private:
        float xOffset, yOffset;
        
        Shader *s;
        Texture2D *tex, *tex2;
        MinecraftClone::Window *window;
        Batch<Vertex> batch;
};

#endif
