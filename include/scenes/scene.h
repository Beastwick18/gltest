#ifndef MINECRAFT_CLONE_SCENE_H
#define MINECRAFT_CLONE_SCENE_H

#include "renderer/renderer.h"

class Scene {
    public:
        virtual ~Scene() {}
        
        virtual void update(double deltaTime) {}
        virtual void render(const Renderer &r) {}
        virtual void guiRender() {}
};

#endif
