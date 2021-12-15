#include <cstring>
#include "core.h"
#include "core/input.h"
#include "core/window.h"
#include "core/renderer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "ClearColorScene.h"
#include "OriginalScene.h"
#include "QuadScene.h"

using namespace MinecraftClone;

int windowWidth = 960, windowHeight = 720;
const char *windowTitle = "Test Window 2: Press space to see something cool!";
bool fullscreen = false, vsync = true;
double targetFramerate = 60.0;

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
    ImGui_ImplGlfw_InitForOpenGL(window->getGlfwWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 130");
    
    double timestep = 1.0/60.0;
    double framestep = 1.0/targetFramerate;
    double currentTime = glfwGetTime();
    double lastTime = currentTime;
    double deltaTime = 0;
    double tickCount = 0;
    double frameCount = 0;
    double timer = lastTime;
    double dt = 0;
    double dtSum = 0;
    
    {
    // OriginalScene originalScene(window);
    // ClearColorScene clearScene(window);
    // QuadScene quadScene(window);
    
    int currentScene = 1;
    Scene *scene = new OriginalScene(window);
    Scene *nextScene = nullptr;
    
    while(!window->shouldClose()) {
        currentTime = glfwGetTime();
        dt = (currentTime - lastTime);
        dtSum += dt;
        deltaTime += dt / timestep;
        lastTime = currentTime;
        
        if(nextScene != nullptr) {
            delete scene;
            scene = nextScene;
            nextScene = nullptr;
        }
        
        
        if(dtSum >= framestep) {
            frameCount++;
            scene->render(renderer);
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            scene->guiRender();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window->getGlfwWindow());
            dtSum = 0;
        }
        
        while(deltaTime >= 1) {
            tickCount++;
            
            scene->update();
            
            if(Input::isKeyDown(GLFW_KEY_ESCAPE))
                window->close();
            
            if(Input::isKeyDown(GLFW_KEY_1) && currentScene != 1) {
                nextScene = new OriginalScene(window);
                currentScene = 1;
            }
            if(Input::isKeyDown(GLFW_KEY_2) && currentScene != 2) {
                nextScene = new ClearColorScene(window);
                currentScene = 2;
            }
            if(Input::isKeyDown(GLFW_KEY_3) && currentScene != 3) {
                nextScene = new QuadScene(window);
                currentScene = 3;
            }
            
            deltaTime--;
        }
        
        
        if(glfwGetTime() - timer > 1) {
            printf("FPS: %f, TICK: %f\n", frameCount, tickCount);
            tickCount = 0;
            frameCount = 0;
            timer++;
        }
        
        
        int display_w, display_h;
        glfwGetFramebufferSize(window->getGlfwWindow(), &display_w, &display_h);
        window->width = display_w;
        window->height = display_h;
        glViewport(0, 0, display_w, display_h);
        
        glfwPollEvents();
    }
    }
    
    Window::freeWindow(window);
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwTerminate();
    
    return 0;
}
