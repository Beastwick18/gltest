#include "scenes/BatchScene3D.h"
#include "imgui/imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include "utils/DebugStats.h"
#include "glm/gtc/noise.hpp"
#include "glm/gtc/random.hpp"

using namespace MinecraftClone;

BatchScene3D::BatchScene3D(Window *window) : window(window) {
    Input::disableCursor();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    s = Shader::createShader("assets/shaders/batch3d.glsl");
    s->use();
    s->setUniform1i("tex0", 0);
    vpUniform = s->getUniformLocation("viewProj");
    modelUniform = s->getUniformLocation("model");
    s->setUniformMat4f(modelUniform, glm::mat4(1.f));
    
    block_atlas = Texture2D::loadFromImageFile("assets/textures/block_atlas.png");
    block_atlas->bind();
    
    int maxChunkX = 20;
    int maxChunkZ = 20;
    
    c = new Camera(window);
    CameraConfig::cameraPos.x = floor((float)maxChunkX / 2.f)*16 + 8.5;
    CameraConfig::cameraPos.z = floor((float)maxChunkZ / 2.f)*16 + 8.5;
    auto pos = glm::floor(CameraConfig::cameraPos);
    CameraConfig::cameraPos.y = ceil(getNoise(pos.x, pos.z))+11;
    c->update(0);
    f = new Frustum(c->getProjection() * c->getView());
    // CameraConfig::noclip = true;
    // CameraConfig::cameraPos.y = 50;
    
    layout.push<float>(3);
    layout.push<float>(3);
    layout.push<float>(2);
    
    Batch<Vertex> b;
    b.init(layout);
    batches.push_back(b);
    
    
    // Create all chunks asynchronously
    int chunkW = 16;
    int chunkL = 16;
    for(int x = 0; x < maxChunkX; x++)
        for(int z = 0; z < maxChunkZ; z++)
            meshFutures.push_back(std::async(std::launch::async, createAllVertices, &meshes, x * chunkW, z * chunkL, chunkW, chunkL));
}

float BatchScene3D::getNoise(float x, float z) {
    glm::vec2 position((float)x/100.f, (float)z/100.f);
    float h = (1+glm::simplex(position))/3.0 * 50;
    // printf("%f\n", h);
    
    float mid = (glm::simplex(glm::vec2(x/20.f,z/20.f)))/2.0 * 5;
    h += mid;
    
    float ext = (glm::simplex(glm::vec2(x/10.f, z/10.f))) * 1.f;
    h += ext;
    
    return h;
}

Quad BatchScene3D::generateQuad(Vertex v0, Vertex v1, Vertex v2, Vertex v3) {
    return { { v0, v1, v2, v2, v3, v0 } };
}

void BatchScene3D::generateCube(std::vector<Quad>& quads, float x, float y, float z, TexCoords topTex, TexCoords bottomTex, TexCoords sideTex, bool top, bool bottom, bool left, bool right, bool front, bool back) {
    if(front) {
        // glm::vec3 norm = glm::cross(v1-v0, v2-v0);
        Vertex v0 = { {x,   y+1, z+1}, {0, 0, -1}, {sideTex.x, sideTex.y+sideTex.h} };
        Vertex v1 = { {x,   y,   z+1}, {0, 0, -1}, {sideTex.x, sideTex.y} };
        Vertex v2 = { {x+1, y,   z+1}, {0, 0, -1}, {sideTex.x+sideTex.w, sideTex.y} };
        Vertex v3 = { {x+1, y+1, z+1}, {0, 0, -1}, {sideTex.x+sideTex.w, sideTex.y+sideTex.h} };
        quads.push_back(generateQuad(v0, v1, v2, v3));
    }
    
    if(left) {
        Vertex v0 = { {x+1, y+1, z+1}, {-1, 0, 0}, {sideTex.x, sideTex.y+sideTex.h} };
        Vertex v1 = { {x+1, y,   z+1}, {-1, 0, 0}, {sideTex.x, sideTex.y} };
        Vertex v2 = { {x+1, y,   z},   {-1, 0, 0}, {sideTex.x+sideTex.w, sideTex.y} };
        Vertex v3 = { {x+1, y+1, z},   {-1, 0, 0}, {sideTex.x+sideTex.w, sideTex.y+sideTex.h} };
        quads.push_back(generateQuad(v0, v1, v2, v3));
    }
    
    if(back)
        quads.push_back(generateQuad(
                { {x,   y+1, z},   {0, 0, 1}, {sideTex.x+sideTex.w, sideTex.y+sideTex.h} },
                { {x+1, y+1, z},   {0, 0, 1}, {sideTex.x, sideTex.y+sideTex.h} },
                { {x+1, y,   z},   {0, 0, 1}, {sideTex.x, sideTex.y} },
                { {x,   y,   z},   {0, 0, 1}, {sideTex.x+sideTex.w, sideTex.y} }
        ));
    
    if(right)
        quads.push_back(generateQuad(
                { {x, y+1, z+1},   {1, 0, 0}, {sideTex.x+sideTex.w, sideTex.y+sideTex.h} },
                { {x, y+1, z},     {1, 0, 0}, {sideTex.x, sideTex.y+sideTex.h} },
                { {x, y,   z},     {1, 0, 0}, {sideTex.x, sideTex.y} },
                { {x, y,   z+1},   {1, 0, 0}, {sideTex.x+sideTex.w, sideTex.y} }
        ));
    
    if(top)
        quads.push_back(generateQuad(
                { {x+1, y+1, z},   {0, 1, 0}, {topTex.x+topTex.w, topTex.y} },
                { {x,   y+1, z},   {0, 1, 0}, {topTex.x, topTex.y} },
                { {x,   y+1, z+1}, {0, 1, 0}, {topTex.x, topTex.y+topTex.h} },
                { {x+1, y+1, z+1}, {0, 1, 0}, {topTex.x+topTex.w, topTex.y+topTex.h} }
        ));
    
    if(bottom)
        quads.push_back(generateQuad(
                { {x,   y, z+1}, {0, -1, 0}, {bottomTex.x, bottomTex.y+bottomTex.h} },
                { {x,   y, z},   {0, -1, 0}, {bottomTex.x, bottomTex.y} },
                { {x+1, y, z},   {0, -1, 0}, {bottomTex.x+bottomTex.w, bottomTex.y} },
                { {x+1, y, z+1}, {0, -1, 0}, {bottomTex.x+bottomTex.w, bottomTex.y+bottomTex.h} }
        ));
}

static std::mutex meshesMutex;
void BatchScene3D::createAllVertices(std::vector<ChunkMesh> *meshes, int startx, int startz, int chunkW, int chunkL) {
    std::vector<Quad> quads;
    
    TexCoords grass { 1.f/32.f, 21.f/32.f, 1.f/32.f, 1.f/32.f };
    TexCoords grassTop { 4.f/32.f, 21.f/32.f, 1.f/32.f, 1.f/32.f };
    TexCoords dirt { 8.f/32.f, 26.f/32.f, 1.f/32.f, 1.f/32.f };
    TexCoords grassBottom = dirt;
    TexCoords stone { 19.f/32.f, 25.f/32.f, 1.f/32.f, 1.f/32.f };
    
    int w = startx + chunkW, l = startz+chunkL, height;
    for(int x = startx; x < w; x++) {
        for(int z = startz; z < l; z++) {
            height = getNoise(x, z)+10;
            float heightLeft = getNoise(x+1, z)+10;
            float heightRight = getNoise(x-1, z)+10;
            float heightFront = getNoise(x, z+1)+10;
            float heightBack = getNoise(x, z-1)+10;
            float stoneHeight = (getNoise(x, z)+10) / 1.15f;
            for(int y = 0; y < height; y++) {
                if(y < stoneHeight-1) {
                    generateCube(quads,
                            x, y, z, stone, stone, stone, 
                            y == height-1, false, heightLeft < y+1, heightRight < y+1,
                            heightFront < y+1, heightBack < y+1
                    );
                } else if(y < height-1) {
                    generateCube(quads,
                            x, y, z, dirt, dirt, dirt,
                            y == height-1, false, heightLeft < y+1, heightRight < y+1,
                            heightFront < y+1, heightBack < y+1
                    );
                } else {
                    generateCube(quads,
                            x, y, z, grassTop, dirt, grass, 
                            y == height-1, false, heightLeft < y+1, heightRight < y+1,
                            heightFront < y+1, heightBack < y+1
                    );
                }
            }
        }
    }
    
    ChunkMesh mesh;
    // mesh.size = quads.size() * 6;
    // mesh.v = (Vertex *)calloc(mesh.size, sizeof(Vertex));
    // mesh.v.push_back(
    mesh.chunkX = startx;
    mesh.chunkZ = startz;
    for(const auto &q : quads)
        for(const auto &v : q.vertices)
            mesh.v.push_back(v);
    std::lock_guard<std::mutex> lock(meshesMutex);
    meshes->push_back(mesh);
    DebugStats::triCount += quads.size() * 3;
}

void BatchScene3D::generateMesh(std::vector<Quad> quads) {
}

BatchScene3D::~BatchScene3D() {
    Shader::freeShader(s);
    Texture2D::free(block_atlas);
    Camera::free(c);
    
    for(auto &b : batches) {
        b.free();
    }
    
    // for(auto &mesh : meshes) {
    //     free(mesh.v);
    // }
}

bool BatchScene3D::drawVertexArray(const Vertex *array, const int size) {
    if(batches.size() > 0) {
        for(auto &b : batches) {
            if(b.hasRoomFor(size)) {
                b.addVertices(array, size);
                return true;
            }
        }
    }
    printf("New batch\n");
    
    Batch<Vertex> newBatch;
    newBatch.init(layout);
    batches.push_back(newBatch);
    
    if(newBatch.hasRoomFor(size)) {
        newBatch.addVertices(array, size);
        return true;
    }
    fprintf(stderr, "Vertex array too big\n");
    return false;
}

bool BatchScene3D::drawCube(glm::vec3 position, glm::vec2 texCoords) {
    // Vertex p1{ position, texCoords };
    // Vertex p2;
    // Vertex p3;
    // Vertex p4;
    // if(batches.size() > 0) {
        
    // }
    
    // return false;
    return false;
}

// TODO: make a cube generator that takes a position + textures and creates all
// the proper vertices

void BatchScene3D::render(const Renderer &r) {
    glClearColor(0.35f, 0.52f, 0.95f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    double flushStart = glfwGetTime();
    if(batches.size() > 0) {
        // Flush all the batches
        s->setUniformMat4f(vpUniform, c->getProjection() * c->getView());
        for(auto &b : batches)
            b.flush();
    }
    DebugStats::flushTime += glfwGetTime() - flushStart;
}

void BatchScene3D::guiRender() {
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
}

void BatchScene3D::update(double deltaTime) {
    c->update(deltaTime);
    
    double drawStart = glfwGetTime();
    for(const auto &mesh : meshes) {
        if(f->isBoxVisible(glm::vec3(mesh.chunkX, 0, mesh.chunkZ), glm::vec3(mesh.chunkX+16, 50, mesh.chunkZ+16)))
            drawVertexArray(mesh.v.data(), mesh.v.size());
    }
    DebugStats::drawTime += glfwGetTime() - drawStart;
    
    if(Input::isKeyBeginDown(GLFW_KEY_X)) {
        auto pos = glm::floor(CameraConfig::cameraPos);
        float x = pos.x;
        float y = pos.y;
        float z = pos.z;
        int chunkX = x/16;
        int chunkZ = z/16;
        
        TexCoords wood = { 8.f/32.f, 16.f/32.f, 1.f/32.f, 1.f/32.f };
        
        std::vector<Quad> quads;
        generateCube(quads, x, y, z, wood, wood, wood, true, true, true, true, true, true);
        
        for(auto &mesh : meshes) {
            if(mesh.chunkX/16 == chunkX && mesh.chunkZ/16 == chunkZ) {
                for(const auto &quad : quads) {
                    for(const auto &v : quad.vertices) {
                        mesh.v.push_back(v);
                    }
                }
            }
        }
    }
    
    // Make player stand on terrain
    auto pos = glm::floor(CameraConfig::cameraPos);
    CameraConfig::ground = ceil(getNoise(pos.x, pos.z))+11;
    
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
    
    if(wiremesh != wiremeshToggle) {
        if(wiremeshToggle)
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        else
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        wiremesh = wiremeshToggle;
    }
    DebugStats::batchCount = batches.size();
    f->update(c->getProjection() * c->getView());
}
