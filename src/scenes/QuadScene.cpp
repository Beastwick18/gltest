#include "QuadScene.h"

QuadScene::QuadScene(MinecraftClone::Window *window) : Scene(window) {
    float vertices[] = {
        -0.5f,  0.5f, // Top left
         0.5f,  0.5f, // Top right
         0.5f, -0.5f, // Bottom right
        -0.5f, -0.5f  // Bottom left
    };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    vao = new VAO;
    vao->bind();
    vbo = new VBO(vertices, sizeof(vertices));
    VBlayout layout;
    layout.push<float>(2); // Push position
    vao->addBuffer(vbo, &layout);
    ebo = new EBO(indices, 6);
    
    s = Shader::createShader("shaders/quad.frag", "shaders/quad.vert");
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
