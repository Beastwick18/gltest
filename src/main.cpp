#include <cstring>
#include "core.h"
#include "input/input.h"
#include "core/window.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "scenes/ClearColorScene.h"
#include "scenes/OriginalScene.h"
#include "scenes/QuadScene.h"
#include "scenes/BatchScene3D.h"
#include "utils/DebugStats.h"

using namespace MinecraftClone;

int windowWidth = 960, windowHeight = 720;
const char *windowTitle = "Voxel Game: Very original!";
bool fullscreen = false, vsync = true;
double targetFramerate = 120.0;

void readArguments(int argc, char **argv) {
    for(int i = 1; i < argc; i++) {
        char *str = argv[i];
        size_t len = strlen(str);
        if(len >= 3 && str[0] == '-' && str[1] == '-') {
            char *commandString = &str[2]; // Remove "--"
            if(std::strcmp(commandString, "fullscreen") == 0) {
                fullscreen = true;
            } else if(std::strcmp(commandString, "windowed") == 0) {
                fullscreen = false;
            } else if(std::strcmp(commandString, "novsync") == 0) {
                vsync = false;
            } else if(std::strcmp(commandString, "vsync") == 0) {
                vsync = true;
            } else { // Check for "--command=value" commands
                size_t commandLen = strlen(commandString);
                char *idx = std::find(commandString, commandString+commandLen, '=');
                if(idx == commandString+commandLen) {
                    printf("Not recognized: %s\n", commandString);
                    continue;
                }
                *idx = 0;
                char *value = idx+1;
                if(std::strcmp(commandString, "width") == 0) {
                    windowWidth = atoi(value);
                } else if(std::strcmp(commandString, "height") == 0) {
                    windowHeight = atoi(value);
                } else if(std::strcmp(commandString, "fps") == 0) {
                    targetFramerate = atoi(value);
                } else {
                    printf("Not recognized: %s\n", commandString);
                }
            }
        }
    }
}

int main(int argc, char **argv) {
    if(!glfwInit()) {
        fprintf(stderr, "Fatal: Failed to init glfw [failed in %s at line %d]\n", __FILE__, __LINE__ - 1);
        return -1;
    }
    readArguments(argc, argv);
    
    Window *window = Window::createWindow(windowWidth, windowHeight, windowTitle, fullscreen);
    // Window *window = Window::createWindow(960, 720, "test", false);
    if(window == nullptr) {
        fprintf(stderr, "Fatal: Failed to create window [failed in %s at line %d]\n", __FILE__, __LINE__ - 2);
        return -1;
    }
    glfwSwapInterval(vsync);
    
    window->setCallbacks();
    
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        fprintf(stderr, "Fatal: Failed to init glad [failed in %s at line %d]\n", __FILE__, __LINE__ - 1);
        glfwTerminate();
        Window::freeWindow(window);
        return -1;
    }
    
    // Setup blending for textures that contain transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glEnable(GL_BLEND);
    
    Renderer::init(window);
    
    
    VAO *vao = new VAO;
    vao->bind();
    VBO *vbo = new VBO(16*sizeof(float));
    VBlayout layout;
    layout.push<float>(2); // Push position
    vao->addBuffer(vbo, layout);
    EBO *ebo = new EBO(12);
    ebo->bind();
    
    Shader *qs = Shader::createShader("assets/shaders/quad.glsl");
    
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window->getGlfwWindow(), false);
    ImGui_ImplOpenGL3_Init("#version 130");
    
    // {
    // int currentScene = 5;
    // Scene *scene = new BatchScene3D(window);
    BatchScene3D scene(window);
    // Scene *nextScene = nullptr;
    
    double targetFps = 1.0/targetFramerate;
    double lastTime = glfwGetTime();
    double dtSum = 0;
    
    while(!window->shouldClose()) {
        double currentTime = glfwGetTime();
        double deltaTime = (currentTime - lastTime);
        lastTime = currentTime;
        
        DebugStats::updateCount++;
        
        window->pollEvents();
        double updateStart = glfwGetTime();
        scene.update(deltaTime);
        Renderer::update(deltaTime);
        DebugStats::updateTime += glfwGetTime() - updateStart;
        
        dtSum += deltaTime;
        if(dtSum >= targetFps) {
            double renderStart = glfwGetTime();
            scene.render();
            // Renderer::render();
            glClear(GL_DEPTH_BUFFER_BIT);
            
            // float ratio = (float)window->getWidth()/(float)window->getHeight();
            
            DebugStats::renderTime += glfwGetTime() - renderStart;
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            double guiStart = glfwGetTime();
            scene.guiRender();
            double guiEnd = glfwGetTime();
            DebugStats::guiTime += guiEnd - guiStart;
            
            // ImGui::End();
            
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
            window->swapBuffers();
            dtSum = 0;
        }
        
        if(Input::isKeyBeginDown(GLFW_KEY_ESCAPE)) {
            window->close();
        }
        
        // if(Input::isKeyBeginDown(GLFW_KEY_1) && currentScene != 1) {
        //     nextScene = new OriginalScene(window);
        //     currentScene = 1;
        // }
        // if(Input::isKeyBeginDown(GLFW_KEY_2) && currentScene != 2) {
        //     nextScene = new ClearColorScene(window);
        //     currentScene = 2;
        // }
        // if(Input::isKeyBeginDown(GLFW_KEY_3) && currentScene != 3) {
        //     nextScene = new QuadScene(window);
        //     currentScene = 3;
        // }
        // if(Input::isKeyBeginDown(GLFW_KEY_4) && currentScene != 4) {
        //     nextScene = new BatchRendering2DScene(window);
        //     currentScene = 4;
        // }
        // if(Input::isKeyBeginDown(GLFW_KEY_5) && currentScene != 5) {
        //     nextScene = new BatchScene3D(window);
        //     currentScene = 5;
        // }
        
        // if(glfwGetTime() - timer > 1) {
        //     printf("FPS: %f, TICK: %f\n", frameCount, tickCount);
        //     tickCount = 0;
        //     frameCount = 0;
        //     timer++;
        // }
        
        
        int display_w, display_h;
        glfwGetFramebufferSize(window->getGlfwWindow(), &display_w, &display_h);
        if((unsigned)display_w != window->getWidth() || (unsigned)display_h != window->getHeight()) {
            window->resize(display_w, display_h);
            glViewport(0, 0, display_w, display_h);
        }
        Input::reset();
        DebugStats::reset();
    }
    // if(scene != nullptr)
        // delete scene;
    // if(nextScene != nullptr)
    //     delete nextScene;
    
    // }
    
    Renderer::free();
    Window::freeWindow(window);
    
    VAO::free(vao);
    VBO::free(vbo);
    EBO::free(ebo);
    Shader::freeShader(qs);
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwTerminate();
    return 0;
}
