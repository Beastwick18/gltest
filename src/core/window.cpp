#include "core/window.h"
#include "core/input.h"

namespace MinecraftClone {
    Window *Window::createWindow(int width, int height, const char *title, bool fullscreen) {
        // Set glfw version to 4.6
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        // Ensure we don't use deprecated methods
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        Window *w = new Window();
        // Select monitor if fullscreen is on
        GLFWmonitor *monitor = fullscreen ? glfwGetPrimaryMonitor() : nullptr;
        
        w->width = width;
        w->height = height;
        w->title = title;
        
        // Create native glfw window
        w->glfwWindow = glfwCreateWindow(width, height, title, monitor, nullptr);
        if(w->glfwWindow == nullptr) {
            fprintf(stderr, "Fatal: Failed to create GLFW window [failure in %s on line %d]\n", __FILE__, __LINE__ - 2);
            glfwTerminate();
            return nullptr;
        }
        // Make this window the current window we are working with
        glfwMakeContextCurrent(w->glfwWindow);
        
        return w;
    }
    
    void Window::freeWindow(Window *window) {
        if(window != nullptr) {
            glfwDestroyWindow(window->glfwWindow);
            delete window;
        }
    }
    
    void Window::close() {
        if(glfwWindow != nullptr) {
            glfwSetWindowShouldClose(glfwWindow, GLFW_TRUE);
        }
    }
    
    void Window::setCallbacks() {
        if(glfwWindow != nullptr) {
            glfwSetKeyCallback(glfwWindow, Input::keyCallback);
            glfwSetMouseButtonCallback(glfwWindow, Input::mouseButtonCallback);
            glfwSetCursorPosCallback(glfwWindow, Input::mouseCallback);
            glfwSetScrollCallback(glfwWindow, Input::mouseScrollCallback);
        } else {
            fprintf(stderr, "Non fatal: Cannot set callbacks, glfw window not created yet [line %d]\n", __LINE__);
        }
    }
    
    bool Window::shouldClose() {
        return glfwWindowShouldClose(glfwWindow);
    }
    
    GLFWwindow *Window::getGlfwWindow() {
        return glfwWindow;
    }
}
