#include "OriginalScene.h"
#include "core/input.h"
#include "core/texture2D.h"
#include "core/VAO.h"
#include "core/VBO.h"
#include "core/EBO.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"

using namespace MinecraftClone;

OriginalScene::OriginalScene(Window *window) : Scene(window) {
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    this->wiremesh = false;
    this->wiremeshToggle = false;
    this->window = window;
    Input::disableCursor();
    // Bind the output of fragment shader to "outColor"
    s = Shader::createShader("shaders/original.glsl");
    s->bindFragDataLocation(0, "outColor");
    
    float vertices[] = {
         //   Position   // Tex coords //
         0.0f,  0.5f, 1.0f, 0.0f, 1.0f, // Top left
         1.0f,  0.5f, 1.0f, 1.0f, 1.0f, // Top right
         1.0f, -0.5f, 1.0f, 1.0f, 0.0f, // Bottom right
         0.0f, -0.5f, 1.0f, 0.0f, 0.0f, // Bottom left
        
         0.0f,  0.5f, 0.0f, 1.0f, 1.0f, // Top left back
         1.0f,  0.5f, 0.0f, 0.0f, 1.0f, // Top right back
        
         1.0f, -0.5f, 0.0f, 0.0f, 0.0f, // Bottom right back
         0.0f, -0.5f, 0.0f, 1.0f, 0.0f, // Bottom left back
        
         1.0f, -0.5f, 0.0f, 1.0f, 1.0f, // Bottom right back
         0.0f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom left back
        
         0.0f,  0.5f, 0.0f, 0.0f, 0.0f, // Top left back
         1.0f,  0.5f, 0.0f, 1.0f, 0.0f, // Top right back
    };
    
    unsigned int indices[] = {
        // Front face
        0, 2, 1,
        0, 3, 2,
        
        // Top face
        0, 1, 10,
        1, 11, 10,
        
        // Left face
        // 0, 4, 7,
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
        // 4, 5, 6,
        7, 4, 5,
    };
    
    vao = new VAO;
    vao->bind();
    vbo = new VBO(vertices, sizeof(vertices));
    VBlayout layout;
    layout.push<float>(3);
    layout.push<float>(2);
    vao->addBuffer(vbo, &layout);
    
    ebo = new EBO(indices, 36);
    
    tex = Texture2D::loadFromImageFile("res/dirt.png");
    tex2 = Texture2D::loadFromImageFile("res/pack.png");
    tex3 = Texture2D::loadFromImageFile("res/diamond_block.png");
    
    test = s->getUniformLocation("test");
    mvpUniform = s->getUniformLocation("MVP");
    
    // Translate the model by .5, .5
    model = glm::mat4(1.0f);
    
    camera = new Camera(window, glm::vec3(0,0,0), .1f,.1f,  90.0f, 70.0f, 120.0f);
    Input::installCamera(camera);
    camera->update();
    
    // Initialize some uniforms
    s->use();
    s->setUniformMat4f(mvpUniform, model);
    s->setUniform1i("tex0", 0);
    s->setUniform1i("tex1", 1);
    
    l = 0;
    mouseAlreadyPressed = false;
    keyAlreadyPressed = false;
    playAnimation = true;
    count = 0;
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
    Input::uninstallCamera();
    delete camera;
}

bool f_pressed = false;
bool c_pressed = false;
bool z_pressed = false;
float oldFov = 0;
void OriginalScene::update() {
    camera->update();
    if(playAnimation)
        count += .01f;
    
    l = std::abs(std::sin(count));
    // reset once over 2pi
    if(count > 2*3.14f) count = 0;
    
    bool keyPressed = Input::isKeyDown(GLFW_KEY_TAB);
    if(keyPressed && !keyAlreadyPressed) {
        if(Input::cursorEnabled)
            Input::disableCursor();
        else
            Input::enableCursor();
    }
    keyAlreadyPressed = keyPressed;
    keyPressed = Input::isKeyDown(GLFW_KEY_F);
    if(!f_pressed && keyPressed)
        window->setFullscreen(!window->isFullscreen());
    f_pressed = keyPressed;
    
    keyPressed = Input::isKeyDown(GLFW_KEY_C);
    if(keyPressed && !c_pressed) {
        oldFov = camera->getFov();
        camera->setFov(50.0f);
    } else if(!keyPressed && c_pressed) {
        camera->setFov(oldFov);
    }
    // else
    //     camera->setFov(120.0f);
    c_pressed = keyPressed;
    
    keyPressed = Input::isKeyDown(GLFW_KEY_Z);
    if(!z_pressed && keyPressed)
        camera->toggleNoclip();
    z_pressed = keyPressed;
    
    if(wiremesh != wiremeshToggle) {
        if(wiremeshToggle)
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        else
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
    wiremesh = wiremeshToggle;
}

void OriginalScene::render(const Renderer &renderer) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    s->use();
    // s->setUniform1f(test, l);
    // s->setUniformMat4f(mvpUniform, mvp);
    int start = -projWidth;
    tex3->bind();
    // tex2->bind(1);
    float time = glfwGetTime();
    int w = 20, l = 20, h = 5;
    float y = -1;
    // for(int y = 0; y < h; y++)
    for(int x = 0; x < w; x++) {
        for(int z = 0; z < l; z++) {
            // float yoff = sin(time*3 + 0.5f*(float)i);
            float yoff = 0;
            if(((x+z) & 1) == 0)
                tex3->bind();
            else
                tex->bind();
            s->setUniformMat4f(mvpUniform, camera->getProjection() * camera->getView() * glm::translate(model, glm::vec3(x, y, z)) );
            // renderer.render(vao, ebo, s);
            
            s->use();
            vao->bind();
            ebo->bind();
            // glDrawElements(GL_TRIANGLES, ebo->getCount(), GL_UNSIGNED_INT, (void*)(0 * sizeof(float)));
            
            // Front
            tex->bind();
            if(z == l-1)
                glDrawElements(GL_TRIANGLES, ebo->getCount()-30, GL_UNSIGNED_INT, (void*)(0 * sizeof(float)));
            // Top
            tex3->bind();
            // if(y == h-1)
                glDrawElements(GL_TRIANGLES, ebo->getCount()-30, GL_UNSIGNED_INT, (void*)(6 * sizeof(float)));
            // Left
            tex->bind();
            if(x == 0)
                glDrawElements(GL_TRIANGLES, ebo->getCount()-30, GL_UNSIGNED_INT, (void*)(12 * sizeof(float)));
            // Right
            tex3->bind();
            if(x == w-1)
                glDrawElements(GL_TRIANGLES, ebo->getCount()-30, GL_UNSIGNED_INT, (void*)(18 * sizeof(float)));
            // Bottom
            tex->bind();
            // if(y == 0)
                glDrawElements(GL_TRIANGLES, ebo->getCount()-30, GL_UNSIGNED_INT, (void*)(24 * sizeof(float)));
            // Back
            tex3->bind();
            if(z == 0)
                glDrawElements(GL_TRIANGLES, ebo->getCount()-30, GL_UNSIGNED_INT, (void*)(30 * sizeof(float)));
            start += 1;
        }
    }
}

void OriginalScene::guiRender() {
    float f = camera->getFov();
    static float sens = .1f;
    
    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
    ImGui::SliderFloat("FOV", &f, 1.0f, 120.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    camera->setFov(f);
    ImGui::SliderFloat("Sensitivity", &sens, 0.0f, 1.0f);
    camera->setSensitivity(sens);
    ImGui::SliderFloat("Bobbing", &camera->bobbing, 0.0f, 1.0f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Checkbox("Wire mesh", &wiremeshToggle);
    ImGui::Checkbox("No clip", &camera->noClip);
    if(ImGui::Button("No clip")) {
        // camera->toggleNoclip();
    }
        
    ImGui::End();
}
