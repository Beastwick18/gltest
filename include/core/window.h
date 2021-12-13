#ifndef MINECRAFT_CLONE_WINDOW_H
#define MINECRAFT_CLONE_WINDOW_H

#include "core.h"

namespace MinecraftClone {
    class Window {
        private:
            GLFWwindow *glfwWindow;
            unsigned int width, height;
            std::string title;
            bool fullscreen;
        public:
            static Window *createWindow(unsigned int width, unsigned int height, const std::string &title, const bool fullscreen = false);
            static void freeWindow(Window *window);
            
            GLFWwindow *getGlfwWindow();
            void close();
            void setCallbacks();
            bool shouldClose();
    };
}

#endif
