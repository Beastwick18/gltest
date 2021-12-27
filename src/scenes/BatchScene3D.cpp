#include "scenes/BatchScene3D.h"
#include "imgui/imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include "utils/DebugStats.h"
#include "glm/gtc/noise.hpp"
#include "world/Chunk.h"
#include "world/World.h"

using namespace MinecraftClone;

BatchScene3D::BatchScene3D(Window *window) : window(window) {
    Input::disableCursor();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // s = Shader::createShader("assets/shaders/batch3d.glsl");
    // s->use();
    // s->setUniform1i("tex0", 0);
    // vpUniform = s->getUniformLocation("viewProj");
    // modelUniform = s->getUniformLocation("model");
    // wave = 0;
    // s->setUniformMat4f(modelUniform, glm::mat4(1.f));
    
    // wavey = Shader::createShader("assets/shaders/waveyBlock.glsl");
    // wavey->use();
    // wavey->setUniform1i("tex0", 0);
    // wVpUniform = wavey->getUniformLocation("viewProj");
    // wModelUniform = wavey->getUniformLocation("model");
    // wave = 0;
    // waveUniform = wavey->getUniformLocation("waveOffset");
    // wavey->setUniformMat4f(wModelUniform, glm::mat4(1.f));
    
    // s->use();
    
    block_atlas = SpriteSheet::loadFromImageFile("assets/textures/block_atlas.png", 32, 32);
    block_atlas->bind();
    
    TexCoords grass = block_atlas->getSubTexture(1, 21);
    TexCoords grassTop = block_atlas->getSubTexture(4, 21);
    TexCoords dirt = block_atlas->getSubTexture(8, 26);
    TexCoords stone = block_atlas->getSubTexture(19, 25);
    TexCoords logSide = block_atlas->getSubTexture(7, 18);
    TexCoords logTop = block_atlas->getSubTexture(8, 18);
    TexCoords leaves = block_atlas->getSubTexture(9, 19);
    TexCoords sand = block_atlas->getSubTexture(18, 27);
    TexCoords water = block_atlas->getSubTexture(20, 19);
    TexCoords lava = block_atlas->getSubTexture(8, 19);
    
    BlockTexture grassTex{ grassTop, dirt, grass, grass, grass, grass };
    BlockTexture dirtTex{ dirt, dirt, dirt, dirt, dirt, dirt };
    BlockTexture stoneTex{ stone, stone, stone, stone, stone, stone };
    BlockTexture logTex{ logTop, logTop, logSide, logSide, logSide, logSide };
    BlockTexture leavesTex{ leaves, leaves, leaves, leaves, leaves, leaves };
    BlockTexture sandTex{ sand, sand, sand, sand, sand, sand };
    BlockTexture waterTex{ water, water, water, water, water, water };
    BlockTexture lavaTex{ lava, lava, lava, lava, lava, lava };
    
    Blocks::blocks[0] = Block{ "air", {}, 0, true, false};
    Blocks::blocks[1] = Block{ "grass", grassTex, 1, false, false };
    Blocks::blocks[2] = Block{ "dirt", dirtTex, 2, false, false };
    Blocks::blocks[3] = Block{ "stone", stoneTex, 3, false, false };
    Blocks::blocks[4] = Block{ "log", logTex, 4, false, false };
    Blocks::blocks[5] = Block{ "leaves", leavesTex, 5, true, false };
    Blocks::blocks[6] = Block{ "sand", sandTex, 6, false, false };
    Blocks::blocks[7] = Block{ "water", waterTex, 7, true, true };
    Blocks::blocks[8] = Block{ "lava", lavaTex, 8, true, true };
    
    int maxChunkX = 20;
    int maxChunkZ = 20;
    
    c = new Camera(window);
    Renderer::setCamera(c);
    CameraConfig::cameraPos.x = floor((float)maxChunkX / 2.f)*16 + 8.5;
    auto pos = glm::floor(CameraConfig::cameraPos);
    CameraConfig::cameraPos.y = ceil(Chunk::getNoise(pos.x, pos.z))+70.7;
    CameraConfig::cameraPos.z = floor((float)maxChunkZ / 2.f)*16 + 8.5;
    c->update(0);
    f = new Frustum(c->getProjection() * c->getView());
    
    layout.push<float>(3);
    layout.push<float>(3);
    layout.push<float>(2);
    
    // Create all chunks asynchronously
    for(int x = 0; x < maxChunkX; x++)
        for(int z = 0; z < maxChunkZ; z++) {
            Chunk c(x,z);
            c.generateChunk();
            World::chunks.push_back(c);
        }
    printf("Test: %d\n", World::chunks[0] < World::chunks[1]);
    for(auto &c : World::chunks) {
        meshFutures.push_back(std::async(std::launch::async, &Chunk::rebuildMesh, &c));
        // c.rebuildMesh();
    }

    test = glm::perspective(glm::radians(180.f), (float)window->getWidth()/10000, 0.1f, 1000.0f);
}

BatchScene3D::~BatchScene3D() {
    SpriteSheet::free(block_atlas);
    Camera::free(c);
    delete f;
}

bool BatchScene3D::drawVertexArray(const Vertex *array, const int size) {
    return false;
}

bool BatchScene3D::drawTransparentVertexArray(const Vertex *array, const int size) {
    return false;
}

void BatchScene3D::render() {
    glClearColor(0.35f, 0.52f, 0.95f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    double drawStart = glfwGetTime();
    for(const auto &c : World::chunks) {
        if(c.getStatus() == ChunkStatus::SHOWING) {
            // continue;
        // auto pos = c.getPos();
        // if(f->isBoxVisible(glm::vec3(pos.x, 0, pos.y), glm::vec3(pos.x + Chunk::chunkW, Chunk::chunkH, pos.y + Chunk::chunkL))) {
            // if(Input::isKeyBeginDown(GLFW_KEY_Q))
            //     printf("YES %d, %d\n", pos.x, pos.y);
            const auto &v = c.getMesh().v;
            DebugStats::chunksRenderedCount++;
            Renderer::renderMesh(v.data(), v.size());
            
            // const auto &tv = c.getTransparentMesh().v;
            // Renderer::renderTransparentMesh(tv.data(), tv.size());
        }
    }
    if(Input::isKeyBeginDown(GLFW_KEY_Q))
        printf("%d, %d\n", window->getWidth(), window->getHeight());
    for(const auto &c : World::chunks) {
        if(c.getStatus() == ChunkStatus::SHOWING) {
            const auto &v = c.getTransparentMesh().v;
            Renderer::renderTransparentMesh(v.data(), v.size());
        }
    }
    DebugStats::drawTime += glfwGetTime() - drawStart;
}

void BatchScene3D::guiRender() {
    if(showGui) {
        ImGui::Begin("Camera Settings");
        if(ImGui::SliderFloat("FOV", &CameraConfig::fov, 1.0f, 120.0f)) {
            c->recalculateProjection();
        }
        ImGui::SliderFloat("Mouse Sensitivity", &CameraConfig::mouseSensitivity, 0.0f, 10.0f);
        ImGui::SliderFloat("Bobbing Height", &CameraConfig::bobbingHeight, 0.0f, 1.0f);
        ImGui::SliderFloat("Bobbing Speed", &CameraConfig::bobbingSpeed, 0.0f, 20.0f);
        ImGui::SliderFloat("Move Speed", &CameraConfig::cameraSpeed, 0.0f, 100.0f);
        ImGui::SliderFloat("Jump Velocity", &CameraConfig::jumpVelocity, 0.0f, 20.0f);
        ImGui::SliderFloat("Gravity", &CameraConfig::gravity, 0.0f, 20.f);
        ImGui::SliderFloat("Ground", &CameraConfig::ground, 0.0f, 100.f);
        ImGui::SliderFloat("Velocity Smoothing", &CameraConfig::lerpSpeed, 0.0f, 20.0f);
        ImGui::SliderFloat("Max Pitch", &CameraConfig::maxPitch, 0.0f, 89.9f);
        ImGui::SliderFloat("Min Pitch", &CameraConfig::minPitch, 0.0f, 89.9f);
        bool pitch = ImGui::SliderFloat("Pitch", &CameraConfig::pitch, -89.9f, 89.9f);
        bool roll = ImGui::SliderFloat("Roll", &CameraConfig::roll, -90.0f, 90.0f);
        bool yaw = ImGui::SliderFloat("Yaw", &CameraConfig::yaw, 0.f, 360.f);
        if(pitch || roll || yaw) {
            CameraConfig::updateRotation();
        }
        ImGui::SliderFloat("Climb Speed", &CameraConfig::climbSpeed, 0.f, 20.f);
        ImGui::SliderFloat3("Camera Position", glm::value_ptr(CameraConfig::cameraPos), -50.f, 50.f);
        
        ImGui::Checkbox("Wire mesh", &wiremeshToggle);
        ImGui::Checkbox("No clip", &CameraConfig::noclip);
        ImGui::End();
    
        ImGui::Begin("Debug stats");
        // ImGui::Text("FPS: %f, TPS: %f\n", (float)framesCount, (float)updateCount);
        ImGui::Text("Render: %f, Update: %f, GUI: %f\n", 1000*DebugStats::renderTime, 1000*DebugStats::updateTime / DebugStats::updateCount, 1000*DebugStats::guiTime);
        ImGui::Text("Draw calls: %u, Draw: %f, Flush: %f\n", DebugStats::drawCalls, 1000*DebugStats::drawTime, 1000*DebugStats::flushTime);
        ImGui::Text("Tri Count: %u\n", DebugStats::triCount);
        ImGui::Text("Chunks Rendered: %u\n", DebugStats::chunksRenderedCount);
        ImGui::Text("%.1f FPS (%.3f ms/frame) ", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
        ImGui::End();
    }
}

// glm::ivec2 prevChunk;
double dtSum = 0;
void BatchScene3D::update(double deltaTime) {
    c->update(deltaTime);
    f->update(c->getProjection() * c->getView());
    
    for(auto &c : World::chunks) {
        if(c.getStatus() != ChunkStatus::SHOWING && c.getStatus() != ChunkStatus::HIDDEN)
            continue;
        
        auto pos = c.getPos();
        if(f->isBoxVisible(glm::vec3(pos.x, 0, pos.y), glm::vec3(pos.x + Chunk::chunkW, Chunk::chunkH, pos.y + Chunk::chunkL))) {
            c.show();
        }else{
            c.hide();
        }
    }
    
    // glm::ivec2 currChunk {((int)CameraConfig::cameraPos.x) / Chunk::chunkW, ((int)CameraConfig::cameraPos.z) / Chunk::chunkL };
    // if(currChunk != prevChunk && World::chunks.size() > 0)
    //     std::sort(World::chunks.begin(), World::chunks.end());
    // prevChunk = currChunk;
    if(Input::isKeyBeginDown(GLFW_KEY_X)) {
        auto pos = glm::floor(CameraConfig::cameraPos);
        float x = pos.x;
        float y = pos.y;
        float z = pos.z;
        World::addBlock(1, x, y, z);
    }
    if(Input::isKeyBeginDown(GLFW_KEY_C)) {
        auto pos = glm::floor(CameraConfig::cameraPos);
        float x = pos.x;
        float y = pos.y-2;
        float z = pos.z;
        World::removeBlock(x, y, z);
    }
    // if(Input::isKeyBeginDown(GLFW_KEY_V)) {
    //     for(auto &c : World::chunks) {
    //         c.rebuildMesh();
    //     }
    // }
    
    // Make player stand on terrain
    auto pos = glm::floor(CameraConfig::cameraPos);
    CameraConfig::ground = ceil(Chunk::getNoise(pos.x, pos.z))+70.7;
    
    if(Input::isKeyBeginDown(GLFW_KEY_TAB)) {
        if(Input::cursorEnabled)
            Input::disableCursor();
        else
            Input::enableCursor();
    }
    if(Input::isKeyBeginDown(GLFW_KEY_F))
        window->setFullscreen(!window->isFullscreen());
    
    if(Input::isKeyBeginDown(GLFW_KEY_Z))
        CameraConfig::toggleNoclip();
    
    if(Input::isKeyBeginDown(GLFW_KEY_F3))
        showGui = !showGui;
    
    if(wiremesh != wiremeshToggle) {
        if((wiremesh = wiremeshToggle))
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        else
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
}
