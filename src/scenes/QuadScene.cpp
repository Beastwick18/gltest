#include "scenes/QuadScene.h"
#include "input/input.h"

using namespace MinecraftClone;

QuadScene::QuadScene(MinecraftClone::Window *window) {
    Input::enableCursor();
    float vertices[] = {
        -0.5f,  0.5f, // Top left
         0.5f,  0.5f, // Top right
         0.5f, -0.5f, // Bottom right
        -0.5f, -0.5f  // Bottom left
    };
    unsigned int indices[] = {
        0, 2, 1,
        2, 0, 3
    };
    
    vao = new VAO;
    vao->bind();
    vbo = new VBO(vertices, sizeof(vertices), GL_STATIC_DRAW);
    VBlayout layout;
    layout.push<float>(2); // Push position
    vao->addBuffer(vbo, layout);
    ebo = new EBO(indices, 6);
    
    s = Shader::createShader("assets/shaders/quad.glsl");
    s->bindFragDataLocation(0, "outColor");
}

QuadScene::~QuadScene() {
    Shader::freeShader(s);
    VBO::free(vbo);
    VAO::free(vao);
    EBO::free(ebo);
}

void QuadScene::render(const Renderer &r) {
    glDisable(GL_DEPTH_TEST);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    r.render(vao, ebo, s);
}
