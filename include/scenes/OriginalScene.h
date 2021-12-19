#ifndef MINECRAFT_CLONE_ORIGINAL_SCENE_H
#define MINECRAFT_CLONE_ORIGINAL_SCENE_H

#include "scenes/scene.h"
#include "glm/glm.hpp"
#include "renderer/shader.h"
#include "renderer/VAO.h"
#include "renderer/VBO.h"
#include "renderer/EBO.h"
#include "renderer/texture2D.h"
#include "renderer/camera.h"

class OriginalScene : public Scene {
    public:
        OriginalScene(MinecraftClone::Window *window);
        ~OriginalScene();
        
        void update(double deltaTime) override;
        void render(const Renderer &r) override;
        void guiRender() override;
    private:
        // float count = 0;
        bool playAnimation = true;
        float l;
        bool mouseAlreadyPressed = false;
        bool keyAlreadyPressed = false;
        int texImage = 0;
        glm::mat4 model;
        float moveSpeed = 0;
        Shader *s;
        float test;
        GLint testUniform, vpUniform, modelUniform;
        float projWidth;
        VBO *vbo;
        VAO *vao;
        EBO *ebo;
        Texture2D *tex, *tex2, *tex3;
        MinecraftClone::Window *window;
        Camera *camera;
        bool wiremesh;
        bool wiremeshToggle;
};

#endif
