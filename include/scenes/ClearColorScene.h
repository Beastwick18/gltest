#ifndef MINECRAFT_CLONE_CLEAR_COLOR_SCENE_H
#define MINECRAFT_CLONE_CLEAR_COLOR_SCENE_H

#include "scenes/scene.h"
#include "core/window.h"

class ClearColorScene : public Scene {
    public:
        ClearColorScene(MinecraftClone::Window *window);
        
        void render() override;
        void guiRender() override;
    private:
        float clearColor[4];
};

#endif
