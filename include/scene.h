#ifndef MINECRAFT_CLONE_SCENE_H
#define MINECRAFT_CLONE_SCENE_H

#include "core/renderer.h"
#include "core/window.h"

class Scene {
    public:
        Scene(MinecraftClone::Window *window) {}
        virtual ~Scene() {}
        
        virtual void update() {}
        virtual void render(const Renderer &r) {}
        virtual void guiRender() {}
};

#endif
