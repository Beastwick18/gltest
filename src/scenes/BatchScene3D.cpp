#include "scenes/BatchScene3D.h"
#include "imgui/imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include "utils/DebugStats.h"
#include "glm/gtc/noise.hpp"
#include "input/input.h"

using namespace MinecraftClone;

BatchScene3D::BatchScene3D(Window *window) : window(window) {
    Input::disableCursor();
    dtSum = 0;
    
    glLineWidth(1.0f);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    for(int i = 1; i < invSize+1; i++)
        inv[i-1] = i;
    
    Blocks::init();
    Blocks::blockAtlas->bind();
    
    invMesh.init(18 * invSize);
    highlightMesh.init(36);
    
    blockInHand = 0;
    
    
    guiScale = 20.f;
    auto camPos = glm::vec3{1,-.5,-1};
    glm::mat4 view = glm::lookAt(camPos, camPos + glm::normalize(glm::vec3{1,-1,1}), glm::vec3{0,1,0});
    blockView = view;
    
    // int maxChunkX = 20;
    // int maxChunkZ = 20;
    int maxChunkX = 10;
    int maxChunkZ = 10;
    // int maxChunkX = 40;
    // int maxChunkZ = 40;
    
    ray.block.hit = false;
    ray.liquid.hit = false;
    
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
            World::chunks[World::generateChunkKey({x * Chunk::chunkW, z * Chunk::chunkL})] = c;
        }
    
    // for(auto &[_, c] : World::chunks) {
    //     c.calculateSkyLighting();
    // }
    for(auto &[_, c] : World::chunks)
        meshFutures.push_back(std::async(std::launch::async, &Chunk::rebuildMesh, &c));
}

BatchScene3D::~BatchScene3D() {
    Blocks::free();
    Camera::free(c);
    delete f;
    invMesh.free();
    highlightMesh.free();
}

SurroundingBlocks invSides {true, false, true, false, false, true};

// TODO: skybox (cubemap), that changes over time
void BatchScene3D::render() {
    // glClearColor(0.35f * Renderer::skyBrightness, 0.52f * Renderer::skyBrightness, 0.95f * Renderer::skyBrightness, 1.0f);
    if(CameraConfig::ortho) {
        glClearColor(0.35f * Renderer::skyBrightness, 0.52f * Renderer::skyBrightness, 0.95f * Renderer::skyBrightness, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        Renderer::renderCubemap();
    }
    
    // glDisable(GL_DEPTH_TEST);
    // glDepthMask(GL_TRUE);
    // glDepthMask(GL_FALSE);
    // glEnable(GL_DEPTH_TEST);
    
    double drawStart = glfwGetTime();
    // const auto &hitChunk = World::toChunkCoords(ray.block.hitCoords);
    for(const auto &[_, c] : World::chunks) {
        // if(ray.block.hit) {
        if(c.getStatus() == ChunkStatus::SHOWING) {
            const auto &v = c.getMesh();
            DebugStats::chunksRenderedCount++;
            Renderer::renderMesh(v.data, v.size());
        }
    }
    for(const auto &[_, c] : World::chunks) {
        if(c.getStatus() == ChunkStatus::SHOWING) {
            const auto &v = c.getTransparentMesh();
            Renderer::renderTransparentMesh(v.data, v.size());
        }
    }
    DebugStats::drawTime += glfwGetTime() - drawStart;
    
    Renderer::render();
    
    if(!Input::isKeyDown(GLFW_KEY_X)) {
        glClear(GL_DEPTH_BUFFER_BIT);
        // glDisable(GL_DEPTH_TEST);
        // glDepthMask(GL_TRUE);
        
        if(ray.block.hit) {
            auto pos = ray.block.hitCoords;
            SurroundingBlocks adj = World::getAdjacentBlocks(pos);
            adj.top *= !Blocks::getBlockFromID(adj.top).liquid;
            adj.bottom *= !Blocks::getBlockFromID(adj.bottom).liquid;
            adj.left *= !Blocks::getBlockFromID(adj.left).liquid;
            adj.right *= !Blocks::getBlockFromID(adj.right).liquid;
            adj.front *= !Blocks::getBlockFromID(adj.front).liquid;
            adj.back *= !Blocks::getBlockFromID(adj.back).liquid;
            // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            highlightMesh.clear();
            unsigned int triCount = DebugStats::triCount;
            Renderer::generateCubeMesh(highlightMesh, pos, Blocks::highlight, !adj, 1.f, 1.f);
            Renderer::renderMesh(highlightMesh.data, highlightMesh.size());
            DebugStats::triCount = triCount;
            Renderer::flushRegularBatch();
            // glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        }
        // if(c.getPos() == hitChunk && ray.block.hit) {
        glDisable(GL_CULL_FACE);
        invMesh.clear();
        for(int i = 0; i < invSize; i++) {
            unsigned int triCount = DebugStats::triCount;
            const Block &b = Blocks::getBlockFromID(inv[i]);
            if(b.liquid)
                Renderer::generateLiquidMesh(invMesh, {-i * 1.1f, ( blockInHand == i ) * .5f, i * 1.1f}, b.tex, invSides);
            else
                Renderer::generateCubeMesh(invMesh, {-i * 1.1f, ( blockInHand == i ) * .5f, i * 1.1f}, b.tex, invSides);
            DebugStats::triCount = triCount;
        }
        Renderer::renderMesh(invMesh.data, invMesh.size());
        Renderer::regularShader->use();
        float ratio = (float)window->getWidth() / window->getHeight();
        glm::mat4 proj = glm::ortho(0.f, ratio*guiScale, 0.f, guiScale, -1.5f, 0.5f);
        Renderer::regularShader->setUniformMat4f(Renderer::vpUniform, proj * blockView);
        Renderer::regularBatch.flush();
        glEnable(GL_CULL_FACE);
        
        // glEnable(GL_DEPTH_TEST);
        // glDepthMask(GL_TRUE);
    if(!CameraConfig::ortho)
        Renderer::renderCrosshair();
    }
    // glClear(GL_DEPTH_BUFFER_BIT);
    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    // glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void BatchScene3D::guiRender() {
    if(!showGui) return;
    if(ImGui::CollapsingHeader("Camera Settings")) {
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
        bool yaw = ImGui::SliderFloat("Yaw", &CameraConfig::yaw, 0.f, 360.f);
        if(pitch || yaw) {
            CameraConfig::updateRotation();
        }
        ImGui::SliderFloat("Climb Speed", &CameraConfig::climbSpeed, 0.f, 20.f);
        ImGui::SliderFloat3("Camera Position", glm::value_ptr(CameraConfig::cameraPos), -50.f, 50.f);
        // ImGui::SliderInt("Test Position", &test, -5, 5);
        ImGui::SliderFloat("Gui scale", &guiScale, 0.f, 100.f);
        ImGui::SliderFloat("Sky Brightness", &Renderer::skyBrightness, 0.f, 1.f);
        ImGui::SliderInt("Block Reach", &CameraConfig::blockReach, 0.f, 100.f);
        ImGui::SliderFloat("Ortho Zoom", &CameraConfig::orthoZoom, 0.f, 500.f);
        
        ImGui::Checkbox("Wire mesh", &wiremeshToggle);
        ImGui::Checkbox("Ortho", &CameraConfig::ortho);
        ImGui::Checkbox("No clip", &CameraConfig::noclip);
    }
    if(ImGui::CollapsingHeader("Debug stats")) {
        ImGui::Text("Render: %f, Update: %f, GUI: %f\n", 1000*DebugStats::renderTime, 1000*DebugStats::updateTime / DebugStats::updateCount, 1000*DebugStats::guiTime);
        ImGui::Text("Draw calls: %u, Draw: %f, Flush: %f\n", DebugStats::drawCalls, 1000*DebugStats::drawTime, 1000*DebugStats::flushTime);
        ImGui::Text("Tri Count: %u\n", DebugStats::triCount);
        ImGui::Text("Chunks Rendered: %u\n", DebugStats::chunksRenderedCount);
        ImGui::Text("%.1f FPS (%.3f ms/frame) ", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
        ImGui::Text("Focused Block: Blocks::%s\n", Blocks::getBlockFromID(ray.block.blockID).name.c_str());
        ImGui::Text("Focused Liquid: Blocks::%s\n", Blocks::getBlockFromID(ray.liquid.blockID).name.c_str());
        ImGui::Text("Block in hand: Blocks::%s\n", Blocks::getBlockFromID(inv[blockInHand]).name.c_str());
    }
}

bool underwater = false;
float oldG = CameraConfig::gravity;
float oldJ = CameraConfig::jumpVelocity;
float oldM = CameraConfig::cameraSpeed;
void BatchScene3D::update(double deltaTime) {
    c->update(deltaTime);
    f->update(c->getProjection() * c->getView());
    
    for(auto &[_, c] : World::chunks) {
        if(c.getStatus() != ChunkStatus::SHOWING && c.getStatus() != ChunkStatus::HIDDEN)
            continue;
        
        if(c.isVisible(f))
            c.show();
        else
            c.hide();
    }
    
    // Make player stand on terrain (this is stupid)
    auto pos  = glm::floor(CameraConfig::cameraPos - glm::vec3(-.4, 1, -.4));
    auto pos2 = glm::floor(CameraConfig::cameraPos - glm::vec3(.4f, 1, -.4f));
    auto pos3 = glm::floor(CameraConfig::cameraPos - glm::vec3(.4f, 1, .4f));
    auto pos4 = glm::floor(CameraConfig::cameraPos - glm::vec3(-.4f, 1, .4f));
    auto ground = ceil(Chunk::getNoise(pos.x, pos.z)) + 70.7;
    auto ground1 = ceil(Chunk::getNoise(pos2.x, pos2.z))+70.7;
    auto ground2 = ceil(Chunk::getNoise(pos3.x, pos3.z))+70.7;
    auto ground3 = ceil(Chunk::getNoise(pos4.x, pos4.z))+70.7;
    CameraConfig::ground = glm::max(glm::max(ground, ground1), glm::max(ground2, ground3));
    // underwater = Blocks::getBlockFromID(World::getBlock(pos)).liquid || Blocks::getBlockFromID(World::getBlock(pos2)).liquid || Blocks::getBlockFromID(World::getBlock(pos3)).liquid || Blocks::getBlockFromID(World::getBlock(pos4)).liquid;
    // if(underwater) {
    //     CameraConfig::jumpVelocity = oldJ / 2.f;
    //     CameraConfig::gravity = oldG / 2.f;
    //     CameraConfig::cameraSpeed = oldM / 2.f;
    //     CameraConfig::jumping = false;
    // } else {
    //     CameraConfig::jumpVelocity = oldJ;
    //     CameraConfig::gravity = oldG;
    //     CameraConfig::cameraSpeed = oldM;
    // }
    
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
    
    for(int i = 0; i < 9; i++)
        if(Input::isKeyBeginDown(GLFW_KEY_1+i)) {
            blockInHand = i;
        }
    
    if(!Input::cursorEnabled) {
        // TODO: https://antongerdelan.net/opengl/raycasting.html
        // Raycast point and click when in ortho mode
        ray = World::raycast(CameraConfig::cameraPos, CameraConfig::cameraFront, CameraConfig::blockReach);
        if(!CameraConfig::ortho) {
            if(Input::mouseScrollY > 0)
                blockInHand--;
            else if(Input::mouseScrollY < 0)
                blockInHand++;
        }
        
        if(blockInHand > invSize-1) blockInHand = 0;
        else if(blockInHand < 0) blockInHand = invSize-1;
        
        if(ray.block.hit) {
            if(Input::isMouseButtonBeginDown(GLFW_MOUSE_BUTTON_LEFT) && Blocks::getBlockFromID(ray.block.blockID).breakable)
                World::removeBlock(ray.block.hitCoords);
            else if(Input::isMouseButtonBeginDown(GLFW_MOUSE_BUTTON_RIGHT))
                World::addBlock(inv[blockInHand], ray.block.hitCoords + ray.block.hitSide);
            else if(Input::isMouseButtonBeginDown(GLFW_MOUSE_BUTTON_3))
                inv[blockInHand] = ray.block.blockID;
        }
    }
    if(CameraConfig::ortho)
        CameraConfig::orthoZoom += Input::mouseScrollY * -5;
}
