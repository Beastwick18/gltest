#include "scenes/BatchScene3D.h"
#include "imgui/imgui.h"
#include "glm/gtc/type_ptr.hpp"

using namespace MinecraftClone;

bool wiremeshToggle = false, wiremesh = false;
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
    
    c = new Camera(window);
    
    layout.push<float>(3);
    layout.push<float>(2);
    
    Batch<Vertex> b;
    b.init(layout);
    batches.push_back(b);
    printf("%lu\n", batches.size());
    createAllVertices();
}

void BatchScene3D::createAllVertices() {
    float tx = 16.f/512.f;
    float ty = 336.f/512.f;
    float tw = 16.f/512.f;
    float th = 16.f/512.f;
    
    float topx = tx + 48.f/512.f;
    float topy = ty;
    float bottomx = 128.f/512.f;
    float bottomy = 416.f/512.f;
    
    int w = 250, l = 30, h = 30;
    for(int y = 0; y < h; y++) {
        for(int x = 0; x < w; x++) {
            for(int z = 0; z < l; z++) {
                // float yoff = sin(time*3 + 0.5f*(float)x);
                // if(z != l-1 && z != 0 && x != w-1 && x != 0 && y != h-1 && y != 0)
                //     continue;
                
                if(z == l-1)
                    quads.push_back({
                        {
                            { {x,   y+1, z+1}, {tx, ty+th} },
                            { {x,   y,   z+1}, {tx, ty} },
                            { {x+1, y,   z+1}, {tx+tw, ty} },
                            
                            { {x,   y+1, z+1}, {tx, ty+th} },
                            { {x+1, y,   z+1}, {tx+tw, ty} },
                            { {x+1, y+1, z+1}, {tx+tw, ty+th} },
                        }
                    });
                
                if(x == w-1)
                    quads.push_back({
                        {
                            { {x+1, y+1, z+1}, {tx, ty+th} },
                            { {x+1, y,   z+1}, {tx, ty} },
                            { {x+1, y,   z}, {tx+tw, ty} },
                            
                            { {x+1, y+1, z+1}, {tx, ty+th} },
                            { {x+1, y,   z}, {tx+tw, ty} },
                            { {x+1, y+1, z}, {tx+tw, ty+th} },
                        }
                    });
                
                if(z == 0)
                    quads.push_back({
                        {
                            { {x,   y+1, z}, {tx+tw, ty+th} },
                            { {x+1, y+1, z}, {tx, ty+th} },
                            { {x+1, y,   z}, {tx, ty} },
                            
                            { {x,   y+1, z}, {tx+tw, ty+th} },
                            { {x+1, y,   z}, {tx, ty} },
                            { {x,   y,   z}, {tx+tw, ty} },
                        }
                    });
                
                if(x == 0)
                    quads.push_back({
                        {
                            { {x, y+1, z+1}, {tx+tw, ty+th} },
                            { {x, y,   z}, {tx, ty} },
                            { {x, y,   z+1}, {tx+tw, ty} },
                            
                            { {x, y+1, z+1}, {tx+tw, ty+th} },
                            { {x, y+1, z}, {tx, ty+th} },
                            { {x, y,   z}, {tx, ty} },
                        }
                    });
                
                if(y == h-1)
                    quads.push_back({
                        {
                            { {x,   y+1, z}, {topx, topy} },
                            { {x,   y+1, z+1}, {topx, topy+th} },
                            { {x+1, y+1, z}, {topx+tw, topy} },
                            
                            { {x+1, y+1, z+1}, {topx+tw, topy+th} },
                            { {x+1, y+1, z}, {topx+tw, topy} },
                            { {x,   y+1, z+1}, {topx, topy+th} },
                        }
                    });
                
                if(y == 0)
                    quads.push_back({
                        {
                            { {x,   y, z}, {bottomx, bottomy} },
                            { {x+1, y, z}, {bottomx+tw, bottomy} },
                            { {x,   y, z+1}, {bottomx, bottomy+th} },
                            
                            { {x+1, y, z+1}, {bottomx+tw, bottomy+th} },
                            { {x,   y, z+1}, {bottomx, bottomy+th} },
                            { {x+1, y, z}, {bottomx+tw, bottomy} },
                        }
                    });
            }
        }
    }
}

BatchScene3D::~BatchScene3D() {
    Shader::freeShader(s);
    Texture2D::free(block_atlas);
    Camera::free(c);
    
    for(auto &b : batches) {
        b.free();
    }
}

bool BatchScene3D::drawVertexArray(Vertex *array, const int size) {
    if(batches.size() > 0) {
        for(auto &b : batches) {
            if(b.hasRoomFor(size)) {
                for(int i = 0; i < size; i++)
                    b.addVertex(array[i]);
                return true;
            }
        }
    }
    printf("New batch\n");
    
    Batch<Vertex> newBatch;
    newBatch.init(layout);
    batches.push_back(newBatch);
    
    for(auto &b : batches) {
        if(b.hasRoomFor(size)) {
            for(int i = 0; i < size; i++)
                b.addVertex(array[i]);
            return true;
        }
    }
    fprintf(stderr, "Vertex array too big\n");
    return false;
}

double drawTime = 0, flushTime = 0;
int frames = 0;
void BatchScene3D::render(const Renderer &r) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    float time = glfwGetTime();
    
    frames++;
    double drawStart = glfwGetTime();
    for(auto &q : quads) {
        drawVertexArray(q.vertices, 6);
    }
    drawTime += glfwGetTime() - drawStart;
    
    double flushStart = glfwGetTime();
    if(batches.size() > 0) {
        // Flush all the batches
        s->setUniformMat4f(vpUniform, c->getProjection() * c->getView());
        for(auto &b : batches)
            b.flush();
    }
    flushTime += glfwGetTime() - flushStart;
}

void BatchScene3D::guiRender() {
    ImGui::Begin("Camera Settings");                          // Create a window called "Hello, world!" and append into it.
    ImGui::SliderFloat("FOV", &CameraConfig::fov, 1.0f, 120.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::SliderFloat("Mouse Sensitivity", &CameraConfig::mouseSensitivity, 0.0f, 10.0f);
    ImGui::SliderFloat("Bobbing Height", &CameraConfig::bobbingHeight, 0.0f, 1.0f);
    ImGui::SliderFloat("Bobbing Speed", &CameraConfig::bobbingSpeed, 0.0f, 20.0f);
    ImGui::SliderFloat("Move Speed", &CameraConfig::cameraSpeed, 0.0f, 100.0f);
    ImGui::SliderFloat("Jump Velocity", &CameraConfig::jumpVelocity, 0.0f, 20.0f);
    ImGui::SliderFloat("Gravity", &CameraConfig::gravity, 0.0f, 20.f);
    ImGui::SliderFloat("Ground", &CameraConfig::ground, 0.0f, 5.f);
    ImGui::SliderFloat("Velocity Smoothing", &CameraConfig::lerpSpeed, 0.0f, 20.0f);
    ImGui::SliderFloat("Max Pitch", &CameraConfig::maxPitch, 0.0f, 89.9f);
    ImGui::SliderFloat("Min Pitch", &CameraConfig::minPitch, 0.0f, 89.9f);
    bool pitch = ImGui::SliderFloat("Pitch", &CameraConfig::pitch, -89.9f, 89.9f);
    bool roll = ImGui::SliderFloat("Roll", &CameraConfig::roll, -90.0f, 90.0f);
    bool yaw = ImGui::SliderFloat("Yaw", &CameraConfig::yaw, 0.f, 360.f);
    if(pitch || roll || yaw) {
        CameraConfig::updateRotation();
    }
    ImGui::SliderFloat3("Camera Position", glm::value_ptr(CameraConfig::cameraPos), -50.f, 50.f);
    
    ImGui::Checkbox("Wire mesh", &wiremeshToggle);
    ImGui::Checkbox("No clip", &CameraConfig::noclip);
    ImGui::Text("%.1f FPS (%.3f ms/frame) ", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
    ImGui::End();
}

double elapsed = 0;
void BatchScene3D::update(double deltaTime) {
    c->update(deltaTime);
    
    elapsed += deltaTime;
    if(elapsed >= 5) {
        printf("Num Batches: %lu, Draw: %f, Flush: %f\n", batches.size(), 1000*drawTime/frames, 1000*flushTime/frames);
        printf("Tri Count: %lu\n", quads.size()*2);
        elapsed = 0;
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
    
    if(wiremesh != wiremeshToggle) {
        if(wiremeshToggle)
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        else
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
}
