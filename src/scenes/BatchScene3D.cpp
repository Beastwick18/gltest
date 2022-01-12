#include "scenes/BatchScene3D.h"
#include "imgui/imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include "utils/DebugStats.h"
#include "glm/gtc/noise.hpp"

using namespace MinecraftClone;

BatchScene3D::BatchScene3D(Window *window) : window(window) {
    Input::disableCursor();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    Blocks::init();
    Blocks::blockAtlas->bind();
    
    // int maxChunkX = 20;
    // int maxChunkZ = 20;
    int maxChunkX = 10;
    int maxChunkZ = 10;
    
    ray.hit = false;
    
    c = new Camera(window);
    Renderer::setCamera(c);
    CameraConfig::cameraPos.x = floor((float)maxChunkX / 2.f) * Chunk::chunkW + (float)Chunk::chunkW / 2.f;
    auto pos = glm::floor(CameraConfig::cameraPos);
    CameraConfig::cameraPos.y = ceil(Chunk::getNoise(pos.x, pos.z))+70.7;
    CameraConfig::cameraPos.z = floor((float)maxChunkZ / 2.f) * Chunk::chunkL + (float)Chunk::chunkL / 2.f;
    
    
    f = new Frustum(c->getProjection() * c->getView());
    
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
    Blocks::free();
    Camera::free(c);
    delete f;
}

// std::vector<glm::vec3> testPos;

// bool lookingAtBlock = false;
// int side = -1; // 0=top, 1=bottom, 2=left, 3=right, 4=front, 5=back
// glm::vec3 side;
// glm::vec3 lookingAt;
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
    if(ray.hit) {
        std::vector<Vertex> mesh;
        Renderer::generateCubeMesh(mesh, ray.hitCoords.x, ray.hitCoords.y, ray.hitCoords.z, Blocks::highlight, true, true, true, true, true, true);
        Renderer::renderMesh(mesh.data(), mesh.size());
        DebugStats::triCount -= 6;
        Renderer::flushRegularBatch();
    }
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void BatchScene3D::guiRender() {
    if(!showGui) return;
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
    // bool roll = ImGui::SliderFloat("Roll", &CameraConfig::roll, -90.0f, 90.0f);
    bool yaw = ImGui::SliderFloat("Yaw", &CameraConfig::yaw, 0.f, 360.f);
    // if(pitch || roll || yaw) {
    if(pitch || yaw) {
        CameraConfig::updateRotation();
    }
    ImGui::SliderFloat("Climb Speed", &CameraConfig::climbSpeed, 0.f, 20.f);
    ImGui::SliderFloat3("Camera Position", glm::value_ptr(CameraConfig::cameraPos), -50.f, 50.f);
    ImGui::SliderInt("Block Reach", &CameraConfig::blockReach, 0.f, 100.f);
    
    ImGui::Checkbox("Wire mesh", &wiremeshToggle);
    ImGui::Checkbox("No clip", &CameraConfig::noclip);
    ImGui::Text("Focused Block: Blocks::%s\n", Blocks::getBlockFromID(ray.block).name.c_str());
    ImGui::End();
    
    ImGui::Begin("Debug stats");
    ImGui::Text("Render: %f, Update: %f, GUI: %f\n", 1000*DebugStats::renderTime, 1000*DebugStats::updateTime / DebugStats::updateCount, 1000*DebugStats::guiTime);
    ImGui::Text("Draw calls: %u, Draw: %f, Flush: %f\n", DebugStats::drawCalls, 1000*DebugStats::drawTime, 1000*DebugStats::flushTime);
    ImGui::Text("Tri Count: %u\n", DebugStats::triCount);
    ImGui::Text("Chunks Rendered: %u\n", DebugStats::chunksRenderedCount);
    ImGui::Text("%.1f FPS (%.3f ms/frame) ", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
    ImGui::End();
}

double dtSum = 0;
void BatchScene3D::update(double deltaTime) {
    c->update(deltaTime);
    f->update(c->getProjection() * c->getView());
    
    for(auto &c : World::chunks) {
        if(c.getStatus() != ChunkStatus::SHOWING && c.getStatus() != ChunkStatus::HIDDEN)
            continue;
        
        auto pos = c.getPos();
        if(f->isBoxVisible(glm::vec3(pos.x, 0, pos.y), glm::vec3(pos.x + Chunk::chunkW, Chunk::chunkH, pos.y + Chunk::chunkL)))
            c.show();
        else
            c.hide();
    }
    
    // Make player stand on terrain (this is stupid)
    auto pos  = glm::floor(CameraConfig::cameraPos - glm::vec3(-.4, 1, -.4));
    auto pos2 = glm::floor(CameraConfig::cameraPos - glm::vec3(.4f, 1, -.4f));
    auto pos3 = glm::floor(CameraConfig::cameraPos - glm::vec3(.4f, 1, .4f));
    auto pos4 = glm::floor(CameraConfig::cameraPos - glm::vec3(-.4f, 1, .4f));
    auto ground  = ceil(Chunk::getNoise(pos.x, pos.z)) + 70.7;
    auto ground1 = ceil(Chunk::getNoise(pos2.x, pos2.z))+70.7;
    auto ground2 = ceil(Chunk::getNoise(pos3.x, pos3.z))+70.7;
    auto ground3 = ceil(Chunk::getNoise(pos4.x, pos4.z))+70.7;
    CameraConfig::ground = glm::max(glm::max(ground, ground1), glm::max(ground2, ground3));
    
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
    
    ray = World::raycast(CameraConfig::cameraPos, CameraConfig::cameraFront, CameraConfig::blockReach);
    if(ray.hit && !Input::cursorEnabled) {
        if(Input::isMouseButtonBeginDown(GLFW_MOUSE_BUTTON_LEFT))
            World::removeBlock(ray.hitCoords);
        else if(Input::isMouseButtonBeginDown(GLFW_MOUSE_BUTTON_RIGHT))
            World::addBlock(4, ray.hitCoords + ray.hitSide);
    }
}
