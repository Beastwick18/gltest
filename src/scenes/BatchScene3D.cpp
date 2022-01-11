#include "scenes/BatchScene3D.h"
#include "imgui/imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include "utils/DebugStats.h"
#include "glm/gtc/noise.hpp"
#include "world/Chunk.h"
#include "world/World.h"

using namespace MinecraftClone;

BlockTexture highlight;
BatchScene3D::BatchScene3D(Window *window) : window(window) {
    Input::disableCursor();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    block_atlas = SpriteSheet::loadFromImageFile("assets/textures/block_atlas.png", 32, 32);
    block_atlas->bind();
    
    TexCoords grassSide = block_atlas->getSubTexture(1, 21);
    TexCoords grassTop = block_atlas->getSubTexture(4, 21);
    TexCoords dirt = block_atlas->getSubTexture(8, 26);
    TexCoords stone = block_atlas->getSubTexture(19, 25);
    TexCoords logSide = block_atlas->getSubTexture(7, 18);
    TexCoords logTop = block_atlas->getSubTexture(8, 18);
    TexCoords leaves = block_atlas->getSubTexture(9, 19);
    TexCoords sand = block_atlas->getSubTexture(18, 27);
    TexCoords water = block_atlas->getSubTexture(20, 19);
    TexCoords lava = block_atlas->getSubTexture(8, 19);
    TexCoords square = block_atlas->getSubTexture(30, 13, 2, 2);
    
    highlight = { square };
    
    Blocks::blocks[0] = Block{ "air", {}, 0, true, false};
    Blocks::blocks[1] = Block{ "grass", { grassTop, dirt, grassSide }, 1, false, false };
    Blocks::blocks[2] = Block{ "dirt", { dirt }, 2, false, false };
    Blocks::blocks[3] = Block{ "stone", { stone }, 3, false, false };
    Blocks::blocks[4] = Block{ "log", { logTop, logTop, logSide }, 4, false, false };
    Blocks::blocks[5] = Block{ "leaves", { leaves }, 5, true, false };
    Blocks::blocks[6] = Block{ "sand", { sand }, 6, false, false };
    Blocks::blocks[7] = Block{ "water", { water }, 7, true, true };
    Blocks::blocks[8] = Block{ "lava", { lava }, 8, true, true };
    
    // int maxChunkX = 20;
    // int maxChunkZ = 20;
    int maxChunkX = 10;
    int maxChunkZ = 10;
    
    c = new Camera(window);
    Renderer::setCamera(c);
    CameraConfig::cameraPos.x = floor((float)maxChunkX / 2.f) * Chunk::chunkW + (float)Chunk::chunkW / 2.f;
    auto pos = glm::floor(CameraConfig::cameraPos);
    CameraConfig::cameraPos.y = ceil(Chunk::getNoise(pos.x, pos.z))+70.7;
    CameraConfig::cameraPos.z = floor((float)maxChunkZ / 2.f) * Chunk::chunkL + (float)Chunk::chunkL / 2.f;
    
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
    
    for(auto &c : World::chunks)
        meshFutures.push_back(std::async(std::launch::async, &Chunk::rebuildMesh, &c));
}

BatchScene3D::~BatchScene3D() {
    SpriteSheet::free(block_atlas);
    Camera::free(c);
    delete f;
}

std::vector<glm::vec3> testPos;

bool lookingAtBlock = false;
// int side = -1; // 0=top, 1=bottom, 2=left, 3=right, 4=front, 5=back
glm::vec3 side;
glm::vec3 lookingAt;
void BatchScene3D::render() {
    glClearColor(0.35f, 0.52f, 0.95f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    double drawStart = glfwGetTime();
    for(const auto &c : World::chunks) {
        if(c.getStatus() == ChunkStatus::SHOWING) {
            const auto &v = c.getMesh().v;
            DebugStats::chunksRenderedCount++;
            Renderer::renderMesh(v.data(), v.size());
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
    
    Renderer::render();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    if(lookingAtBlock) {
        std::vector<Vertex> mesh;
        Chunk::generateCubeMesh(mesh, lookingAt.x, lookingAt.y, lookingAt.z, highlight, true, true, true, true, true, true);
        Renderer::renderMesh(mesh.data(), mesh.size());
        Renderer::flushRegularBatch();
    }
    for(auto pos : testPos) {
        std::vector<Vertex> mesh;
        Chunk::generateCubeMesh(mesh, pos.x, pos.y, pos.z, highlight, true, true, true, true, true, true);
        Renderer::renderMesh(mesh.data(), mesh.size());
    }
    Renderer::flushRegularBatch();
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void BatchScene3D::guiRender() {
    if(showGui) {
        ImGui::Begin("Camera Settings");
        if(ImGui::SliderFloat("FOV", &CameraConfig::fov, 1.0f, 120.0f)) {
            c->recalculateProjection();
        }
        ImGui::SliderFloat("Zoom FOV", &CameraConfig::zoomFov, 1.0f, 120.0f);
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
        ImGui::SliderInt("Block Reach", &CameraConfig::blockReach, 0.f, 100.f);
        
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

double dtSum = 0;
bool clr = false;
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
    
    {
    // Make player stand on terrain
    auto pos = glm::floor(CameraConfig::cameraPos - glm::vec3(-.4, 1, -.4));
    auto pos2 = glm::floor(CameraConfig::cameraPos - glm::vec3(.4f, 1, -.4f));
    auto pos3 = glm::floor(CameraConfig::cameraPos - glm::vec3(.4f, 1, .4f));
    auto pos4 = glm::floor(CameraConfig::cameraPos - glm::vec3(-.4f, 1, .4f));
    auto ground = ceil(Chunk::getNoise(pos.x, pos.z))+70.7;
    auto ground1 = ceil(Chunk::getNoise(pos2.x, pos2.z))+70.7;
    auto ground2 = ceil(Chunk::getNoise(pos3.x, pos3.z))+70.7;
    auto ground3 = ceil(Chunk::getNoise(pos4.x, pos4.z))+70.7;
    CameraConfig::ground = glm::max(glm::max(ground, ground1), glm::max(ground2, ground3));
    }
    
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
    
    if(lookingAtBlock && !Input::cursorEnabled) {
        if(Input::isMouseButtonBeginDown(GLFW_MOUSE_BUTTON_LEFT))
            World::removeBlock(lookingAt);
        else if(Input::isMouseButtonBeginDown(GLFW_MOUSE_BUTTON_RIGHT))
            World::addBlock(4, lookingAt + side);
    }
    
    {
    // Ray casting :D
    lookingAtBlock = false;
    
    glm::vec3 tMax, tDelta, pos, startPos, step, dir;
    
    pos = glm::floor(CameraConfig::cameraPos);
    BlockID block = World::getBlock(pos);
    startPos = CameraConfig::cameraPos;
    dir = CameraConfig::cameraFront;
    step = glm::sign(dir);
    
    tMax = (pos + (step * .5f) + .5f - startPos) / dir;
    tDelta = step / dir;
    
    int sside = -1;
    
    if(clr) 
        testPos.clear();
    if(Input::isKeyBeginDown(GLFW_KEY_I))
        clr = !clr;
    bool g = Input::isKeyBeginDown(GLFW_KEY_G);
    if(g) printf("pos: %f %f %f\nstart: %f %f %f\ndir: %f %f %f\nstep: %f %f %f\ntMax %f %f %f\ntDelta %f %f %f\n\n", pos.x, pos.y, pos.z, 
            startPos.x, startPos.y, startPos.z, dir.x, dir.y, dir.z, step.x, step.y, step.z, tMax.x, tMax.y, tMax.z, tDelta.x, tDelta.y, tDelta.z);
    
    int count = 0;
    while(glm::distance(startPos, pos) <= CameraConfig::blockReach && block == Blocks::airBlockID && block != Blocks::nullBlockID) {
        float min = glm::min(tMax.x, glm::min(tMax.y, tMax.z));
        if(tMax.x == min) {
            pos.x += step.x;
            tMax.x += tDelta.x;
            sside = 0;
        } else if(tMax.y == min) {
            pos.y += step.y;
            tMax.y += tDelta.y;
            sside = 1;
        } else {
            pos.z += step.z;
            tMax.z += tDelta.z;
            sside = 2;
        }
        glm::vec3 pos2 = pos;
        if(clr) testPos.push_back(pos2);
        
        block = World::getBlock(pos);
        count++;
    }
    switch(sside) {
        case 0: side = {-step.x, 0.f, 0.f}; break;
        case 1: side = {0.f, -step.y, 0.f}; break;
        case 2: side = {0.f, 0.f, -step.z}; break;
    }
    
    if(g) printf("count: %d\n", count);
    
    if(block != Blocks::nullBlockID && block != Blocks::airBlockID) {
        lookingAtBlock = true;
        lookingAt = pos;
    }
    }
}
