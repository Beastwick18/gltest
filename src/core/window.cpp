#include "core/window.h"
#include "input/input.h"
#include <iostream>

namespace MinecraftClone {
    Window *Window::createWindow(unsigned int width, unsigned int height, const std::string &title, bool fullscreen) {
        // Set glfw version to 4.6
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        // Ensure we don't use deprecated methods
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        // Catch glfw errors
        glfwSetErrorCallback(error_callback);
        
        Window *w = new Window();
        w->width = width;
        w->height = height;
        w->title = title;
        
        // Select monitor if fullscreen is on
        GLFWmonitor *monitor = fullscreen ? glfwGetPrimaryMonitor() : nullptr;
        
        // Create native glfw window
        w->glfwWindow = glfwCreateWindow(width, height, w->title.c_str(), monitor, nullptr);
        if(w->glfwWindow == nullptr) {
            fprintf(stderr, "Fatal: Failed to create GLFW window [failure in %s on line %d]\n", __FILE__, __LINE__ - 2);
            glfwTerminate();
            return nullptr;
        }
        // Make this window the current window we are working with
        glfwMakeContextCurrent(w->glfwWindow);
        
        return w;
    }
    
    void Window::error_callback( int error, const char *msg ) {
        std::string s;
        s = " [" + std::to_string(error) + "] " + msg + '\n';
        std::cerr << s << std::endl;
    }
    
    void Window::freeWindow(Window *window) {
        if(window != nullptr) {
            glfwDestroyWindow(window->glfwWindow);
            delete window;
        }
    }
    
    void Window::close() {
        if(glfwWindow != nullptr)
            glfwSetWindowShouldClose(glfwWindow, GLFW_TRUE);
    }
    
    void Window::setCallbacks() {
        if(glfwWindow != nullptr) {
            glfwSetKeyCallback(glfwWindow, Input::keyCallback);
            glfwSetMouseButtonCallback(glfwWindow, Input::mouseButtonCallback);
            glfwSetCursorPosCallback(glfwWindow, Input::mouseCallback);
            glfwSetScrollCallback(glfwWindow, Input::mouseScrollCallback);
            Input::window = this;
        } else
            fprintf(stderr, "Non fatal: Cannot set callbacks, glfw window not created yet [line %d]\n", __LINE__);
    }
    
    bool Window::shouldClose() {
        return glfwWindow != nullptr && glfwWindowShouldClose(glfwWindow);
    }
    
    GLFWwindow *Window::getGlfwWindow() {
        return glfwWindow;
    }
    
    void Window::swapBuffers() {
        glfwSwapBuffers(glfwWindow);
    }
    
    void Window::pollEvents() {
        glfwPollEvents();
    }
    
    void Window::setFullscreen(bool fullscreen) {
        if(this->fullscreen == fullscreen || glfwWindow == nullptr)
            return;
        
        GLFWmonitor *primMonitor = glfwGetPrimaryMonitor();
        GLFWmonitor *monitor = fullscreen ? primMonitor : nullptr;
        const GLFWvidmode *v = glfwGetVideoMode(primMonitor);
        int x, y, mw, mh;
        glfwGetMonitorWorkarea(primMonitor, &x, &y, &mw, &mh);
        if(!fullscreen)
            glfwSetWindowMonitor(glfwWindow, monitor, x+(mw - width) / 2, y+(mh - height) / 2, width, height, v->refreshRate);
        
        glfwSetWindowMonitor(glfwWindow, monitor, 0, 0, v->width, v->height, v->refreshRate);
        this->fullscreen = fullscreen;
        this->width = v->width;
        this->height = v->height;
        glViewport(0, 0, width, height);
    }
    
    void Window::resize(unsigned int width, unsigned int height) {
        this->width = width;
        this->height = height;
    }
}
