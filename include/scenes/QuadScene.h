#ifndef QUAD_SCENE_H
#define QUAD_SCENE_H

#include "scenes/scene.h"
#include "renderer/renderer.h"
#include "core/window.h"
#include "renderer/EBO.h"
#include "renderer/shader.h"

class QuadScene : public Scene {
    public:
        QuadScene(MinecraftClone::Window *window);
        ~QuadScene() override;
        
        void render() override;
    private:
        VBO *vbo;
        VAO *vao;
        EBO *ebo;
        Shader *s;
};

#endif
