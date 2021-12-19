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
#include "scenes/BatchRendering2DScene.h"
#include "scenes/BatchScene3D.h"

using namespace MinecraftClone;

int windowWidth = 960, windowHeight = 720;
const char *windowTitle = "Test Window 2: Press space to see something cool!";
bool fullscreen = false, vsync = true;
double targetFramerate = 120.0;

void readArguments(int argc, char **argv) {
    for(int i = 0; i < argc; i++) {
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
                bool foundValue = false;
                char *value;
                for(int j = 0; j < commandLen; j++) {
                    if(commandString[j] == '=') {
                        foundValue = true;
                        commandString[j] = 0;
                        value = &commandString[j+1];
                        break;
                    }
                }
                if(!foundValue) {
                    printf("Not recognized: %s\n", commandString);
                    continue;
                }
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
    
    Renderer renderer;
    
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window->getGlfwWindow(), false);
    ImGui_ImplOpenGL3_Init("#version 130");
    
    {
    int currentScene = 5;
    Scene *scene = new BatchScene3D(window);
    Scene *nextScene = nullptr;
    
    double targetFps = 1.0/targetFramerate;
    double lastTime = glfwGetTime();
    double dtSum = 0;
    double dtSum2 = 0;
    int framesCount = 0, updateCount = 0;
    
    double updateTime = 0, renderTime = 0, guiTime = 0;
    
    while(!window->shouldClose()) {
        double currentTime = glfwGetTime();
        double deltaTime = (currentTime - lastTime);
        lastTime = currentTime;
        
        if(nextScene != nullptr) {
            delete scene;
            scene = nextScene;
            nextScene = nullptr;
        }
        
        
        // frameCount++;
        
        updateCount++;
        
        glfwPollEvents();
        double updateStart = glfwGetTime();
        scene->update(deltaTime);
        updateTime += glfwGetTime() - updateStart;
        
        dtSum += deltaTime;
        if(dtSum >= 5) {
            printf("FPS: %f, TPS: %f\n", (float)framesCount/5, (float)updateCount / 5);
            printf("Render: %f, Update: %f, GUI: %f\n", 1000*renderTime / framesCount, 1000*updateTime / updateCount, 1000*guiTime / framesCount);
            renderTime = 0;
            updateTime = 0;
            guiTime = 0;
            framesCount = 0;
            updateCount = 0;
            dtSum = 0;
        }
        
        dtSum2 += deltaTime;
        if(dtSum2 >= targetFps) {
            framesCount++;
            double renderStart = glfwGetTime();
            scene->render(renderer);
            double renderEnd = glfwGetTime();
            renderTime += renderEnd - renderStart;
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            double guiStart = glfwGetTime();
            scene->guiRender();
            double guiEnd = glfwGetTime();
            guiTime += guiEnd - guiStart;
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window->getGlfwWindow());
            dtSum2 = 0;
        }
        
        if(Input::isKeyBeginDown(GLFW_KEY_ESCAPE))
            window->close();
        
        if(Input::isKeyBeginDown(GLFW_KEY_1) && currentScene != 1) {
            nextScene = new OriginalScene(window);
            currentScene = 1;
        }
        if(Input::isKeyBeginDown(GLFW_KEY_2) && currentScene != 2) {
            nextScene = new ClearColorScene(window);
            currentScene = 2;
        }
        if(Input::isKeyBeginDown(GLFW_KEY_3) && currentScene != 3) {
            nextScene = new QuadScene(window);
            currentScene = 3;
        }
        if(Input::isKeyBeginDown(GLFW_KEY_4) && currentScene != 4) {
            nextScene = new BatchRendering2DScene(window);
            currentScene = 4;
        }
        if(Input::isKeyBeginDown(GLFW_KEY_5) && currentScene != 5) {
            nextScene = new BatchScene3D(window);
            currentScene = 5;
        }
        
        // if(glfwGetTime() - timer > 1) {
        //     printf("FPS: %f, TICK: %f\n", frameCount, tickCount);
        //     tickCount = 0;
        //     frameCount = 0;
        //     timer++;
        // }
        
        
        int display_w, display_h;
        glfwGetFramebufferSize(window->getGlfwWindow(), &display_w, &display_h);
        if(display_w != window->getWidth() || display_h != window->getHeight()) {
            window->resize(display_w, display_h);
            glViewport(0, 0, display_w, display_h);
        }
        Input::reset();
        
    }
    if(scene != nullptr)
        delete scene;
    if(nextScene != nullptr)
        delete nextScene;
    
    
    }
    
    Window::freeWindow(window);
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwTerminate();
    
    return 0;
}
