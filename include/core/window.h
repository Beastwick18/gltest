#ifndef MINECRAFT_CLONE_WINDOW_H
#define MINECRAFT_CLONE_WINDOW_H

#include "core.h"

namespace MinecraftClone {
    class Window {
        private:
            GLFWwindow *glfwWindow;
            int width, height;
            const char *title;
        public:
            static Window *createWindow(int width, int height, const char *title, bool fullscreen = false);
            static void freeWindow(Window *window);
            
            GLFWwindow *getGlfwWindow();
            void close();
            void setCallbacks();
            bool shouldClose();
    };
}

#endif
