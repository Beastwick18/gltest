#include "scenes/BatchRendering2DScene.h"
#include "imgui/imgui.h"

using namespace MinecraftClone;

BatchRendering2DScene::BatchRendering2DScene(Window *window) : window(window) {
    MinecraftClone::Input::enableCursor();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    VBlayout layout;
    layout.push<float>(3); // Position
    layout.push<float>(2); // Tex coords
    batch.init(layout);
    s = Shader::createShader("assets/shaders/batch.glsl");
    s->use();
    s->setUniform1i("tex0", 0);
    
    xOffset = 0;
    yOffset = 0;
    
    tex = Texture2D::loadFromImageFile("assets/textures/block_atlas.png");
    // tex2 = Texture2D::loadFromImageFile("assets/textures/dirt.png");
    tex->bind();
}

BatchRendering2DScene::~BatchRendering2DScene() {
    Texture2D::free(tex);
    // Texture2D::free(tex2);
    Shader::freeShader(s);
    batch.free();
}

Vertex staticVertices[6] = {
    { {0.0f, 1.0f, 0.0f}, glm::vec3(0), {128.f/512.f, 464.f/512.f} },
    { {0.0f, 0.0f, 0.0f}, glm::vec3(0), {128.f/512.f, 448.f/512.f} },
    { {1.0f, 0.0f, 0.0f}, glm::vec3(0), {144.f/512.f, 448.f/512.f} },
    
    { {0.0f, 1.0f, 0.0f}, glm::vec3(0), {128.f/512.f, 464.f/512.f} },
    { {1.0f, 0.0f, 0.0f}, glm::vec3(0), {144.f/512.f, 448.f/512.f} },
    { {1.0f, 1.0f, 0.0f}, glm::vec3(0), {144.f/512.f, 464.f/512.f} },
};

void BatchRendering2DScene::render(const Renderer &r) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    s->use();
    
    
    Vertex movingVertices[6] = {
        { {xOffset-1, yOffset+1, 0.0f},glm::vec3(0),  {128.f/512.f, 464.f/512.f} },
        { {xOffset-1, yOffset,   0.0f},glm::vec3(0),  {128.f/512.f, 448.f/512.f} },
        { {xOffset+0, yOffset,   0.0f},glm::vec3(0),  {144.f/512.f, 448.f/512.f} },
                                                     
        { {xOffset-1, yOffset+1, 0.0f},glm::vec3(0),  {128.f/512.f, 464.f/512.f} },
        { {xOffset,   yOffset,   0.0f},glm::vec3(0),  {144.f/512.f, 448.f/512.f} },
        { {xOffset,   yOffset+1, 0.0f},glm::vec3(0),  {144.f/512.f, 464.f/512.f} },
    };
    
    tex->bind();
    for(const auto &v : movingVertices)
        batch.addVertex(v);
    
    // tex2->bind();
    for(const auto &v : staticVertices)
        batch.addVertex(v);
    
    batch.flush();
}

void BatchRendering2DScene::guiRender() {
    ImGui::Begin("Settings");                          // Create a window called "Hello, world!" and append into it.
    ImGui::SliderFloat("X offset", &xOffset, -2.0f, 2.0f);
    ImGui::SliderFloat("Y offset", &yOffset, -2.0f, 2.0f);
    ImGui::Text("%.1f FPS (%.3f ms/frame) ", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
    ImGui::End();
}
