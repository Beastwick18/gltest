#ifndef QUAD_SCENE_H
#define QUAD_SCENE_H

#include "scene.h"
#include "core/renderer.h"
#include "core/window.h"

class QuadScene : public Scene {
    public:
        QuadScene(MinecraftClone::Window *window);
        ~QuadScene();
        
        void render(const Renderer &r) override;
    private:
        VBO *vbo;
        VAO *vao;
        EBO *ebo;
        Shader *s;
};

#endif
