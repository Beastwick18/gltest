#include "core.h"
#include "core/input.h"
#include "core/window.h"

using namespace MinecraftClone;

const int windowWidth = 1280, windowHeight = 720;
const char *windowTitle = "Test Window 2: Electric Boogaloo";

int main() {
    if(!glfwInit()) {
        fprintf(stderr, "Fatal: Failed to init glfw [failed in %s at line %d]\n", __FILE__, __LINE__ - 1);
        return -1;
    }
    
    Window *window = Window::createWindow(windowWidth, windowHeight, windowTitle);
    if(window == nullptr) {
        fprintf(stderr, "Fatal: Failed to create window [failed in %s at line %d]\n", __FILE__, __LINE__ - 2);
        return -1;
    }
    
    window->setCallbacks();
    
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        fprintf(stderr, "Fatal: Failed to init glad [failed in %s at line %d]\n", __FILE__, __LINE__ - 1);
        glfwTerminate();
        return -1;
    }
    
    while(!window->shouldClose()) {
        glClear(GL_COLOR_BUFFER_BIT);
        
        glfwSwapBuffers(window->getGlfwWindow());
        glfwPollEvents();
    }
    
    Window::freeWindow(window);
    glfwTerminate();
}
