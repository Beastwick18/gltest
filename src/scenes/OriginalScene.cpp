#include "scenes/OriginalScene.h"
#include "input/input.h"
#include "renderer/texture2D.h"
#include "renderer/VAO.h"
#include "renderer/VBO.h"
#include "renderer/EBO.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"

using namespace MinecraftClone;

OriginalScene::OriginalScene(Window *window) {
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    this->wiremesh = false;
    this->wiremeshToggle = false;
    this->window = window;
    Input::disableCursor();
    // Bind the output of fragment shader to "outColor"
    s = Shader::createShader("assets/shaders/original.glsl");
    // s->bindFragDataLocation(0, "outColor");
    
    float tx = 128.f/512.f;
    float ty = 448.f/512.f;
    float tw = 16.f/512.f;
    float th = 16.f/512.f;
    float vertices[] = {
         //   Position   // Tex coords //
         0.0f, 1.0f, 1.0f, tx, ty+th, // Top left
         1.0f, 1.0f, 1.0f, tx+tw, ty+th, // Top right
         1.0f, 0.0f, 1.0f, tx+tw, ty, // Bottom right
         0.0f, 0.0f, 1.0f, tx, ty, // Bottom left
        
         0.0f, 1.0f, 0.0f, tx+tw, ty+th, // Top left back
         1.0f, 1.0f, 0.0f, tx, ty+th, // Top right back
        
         1.0f, 0.0f, 0.0f, tx, ty, // Bottom right back
         0.0f, 0.0f, 0.0f, tx+tw, ty, // Bottom left back
        
         1.0f, 0.0f, 0.0f, tx+tw, ty+th, // Bottom right back
         0.0f, 0.0f, 0.0f, tx, ty+th, // Bottom left back
        
         0.0f, 1.0f, 0.0f, tx, ty, // Top left back
         1.0f, 1.0f, 0.0f, tx+tw, ty, // Top right back
    };
    
    unsigned int indices[] = {
        // Front face
        0, 2, 1,
        0, 3, 2,
        
        // Top face
        0, 1, 10,
        1, 11, 10,
        
        // Left face
        4, 7, 3,
        4, 3, 0,
        
        // Right face
        1, 6, 5,
        1, 2, 6,
        
        // Bottom face
        2, 3, 8,
        3, 9, 8,
        
        // Back face
        5, 6, 7,
        7, 4, 5,
    };
    
    vao = new VAO;
    vao->bind();
    vbo = new VBO(vertices, sizeof(vertices), GL_STATIC_DRAW);
    VBlayout layout;
    layout.push<float>(3);
    layout.push<float>(2);
    vao->addBuffer(vbo, layout);
    
    ebo = new EBO(indices, 36);
    
    tex = Texture2D::loadFromImageFile("assets/textures/dirt.png");
    tex2 = Texture2D::loadFromImageFile("assets/textures/pack.png");
    tex3 = Texture2D::loadFromImageFile("assets/textures/block_atlas.png");
    
    test = s->getUniformLocation("test");
    vpUniform = s->getUniformLocation("viewProj");
    modelUniform = s->getUniformLocation("model");
    
    // Translate the model by .5, .5
    model = glm::mat4(1.0f);
    
    // camera = new Camera(window, glm::vec3(0,0,0), .1f,.1f,  90.0f, 70.0f, 120.0f);
    camera = new Camera(window);
    
    // Initialize some uniforms
    s->use();
    // s->setUniformMat4f(mvpUniform, model);
    s->setUniform1i("tex0", 0);
    s->setUniform1i("tex1", 1);
    tex3->bind();
    
    l = 0;
    mouseAlreadyPressed = false;
    keyAlreadyPressed = false;
    playAnimation = true;
    // count = 0;
    texImage = 0;
    moveSpeed = .1f;
}

OriginalScene::~OriginalScene() {
    Shader::freeShader(s);
    VBO::free(vbo);
    VAO::free(vao);
    EBO::free(ebo);
    Texture2D::free(tex);
    Texture2D::free(tex2);
    Texture2D::free(tex3);
    delete camera;
}

float oldFov = 0;
void OriginalScene::update(double deltaTime) {
    // if(playAnimation)
    //     count += .01f;
    
    // l = std::abs(std::sin(count));
    // reset once over 2pi
    // if(count > 2*3.14f) count = 0;
    
    if(Input::isKeyBeginDown(GLFW_KEY_TAB)) {
        if(Input::cursorEnabled)
            Input::disableCursor();
        else
            Input::enableCursor();
    }
    if(Input::isKeyBeginDown(GLFW_KEY_F))
        window->setFullscreen(!window->isFullscreen());
    
    // if(Input::isKeyBeginDown(GLFW_KEY_C)) {
    //     CameraConfig::setFov(50.0f);
    // } else if(!Input::isKeyDown(GLFW_KEY_C)) {
    //     CameraConfig::setFov(oldFov);
    // }
    
    if(Input::isKeyBeginDown(GLFW_KEY_Z))
        CameraConfig::toggleNoclip();
    
    if(wiremesh != wiremeshToggle) {
        if(wiremeshToggle)
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        else
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
    wiremesh = wiremeshToggle;
    
    camera->update(deltaTime);
}

void OriginalScene::render(const Renderer &renderer) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    s->setUniformMat4f(vpUniform, camera->getProjection() * camera->getView());
    
    // s->use();
    // vao->bind();
    // ebo->bind();
    s->setUniformMat4f(modelUniform, glm::translate(model, glm::vec3(0, 0, 0)) );
    // tex2->bind(1);
    float time = glfwGetTime();
    // float time = 1;
    int w = 100, l = 30, h = 30;
    // float y = -1;
    for(int y = 0; y < h; y++) {
        for(int x = 0; x < w; x++) {
            // float yoff = sin(time*3 + 0.5f*(float)x);
            for(int z = 0; z < l; z++) {
                // if(((x+z+y) & 1) == 0)
                //     tex3->bind();
                // else
                //     tex->bind();
                if(x != 0 && x != w-1 && y != 0 && y != h-1 && z != 0 && z != l-1)
                    continue;
                
                s->setUniformMat4f(modelUniform, glm::translate(model, glm::vec3(x, y, z)) );
                
                // TODO: Figure out batch rendering, (combining vertices into one vbo)
                // TODO: Use parameterized line equation [r(t)] built from cameraFront to raycast from the camera to a block
                
                // glDrawElements(GL_TRIANGLES, ebo->getCount(), GL_UNSIGNED_INT, nullptr);
                
                if(z == l-1)
                    glDrawElements(GL_TRIANGLES, ebo->getCount()-30, GL_UNSIGNED_INT, (void*)(0 * sizeof(float)));
                else if(z == 0)
                    glDrawElements(GL_TRIANGLES, ebo->getCount()-30, GL_UNSIGNED_INT, (void*)(30 * sizeof(float)));
                
                if(y == h-1)
                    glDrawElements(GL_TRIANGLES, ebo->getCount()-30, GL_UNSIGNED_INT, (void*)(6 * sizeof(float)));
                else if(y == 0)
                    glDrawElements(GL_TRIANGLES, ebo->getCount()-30, GL_UNSIGNED_INT, (void*)(24 * sizeof(float)));
                
                // Left
                if(x == 0)
                    glDrawElements(GL_TRIANGLES, ebo->getCount()-30, GL_UNSIGNED_INT, (void*)(12 * sizeof(float)));
                else if(x == w-1)
                    glDrawElements(GL_TRIANGLES, ebo->getCount()-30, GL_UNSIGNED_INT, (void*)(18 * sizeof(float)));
            }
        }
    }
}

void OriginalScene::guiRender() {
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
