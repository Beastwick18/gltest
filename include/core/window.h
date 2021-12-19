#ifndef MINECRAFT_CLONE_WINDOW_H
#define MINECRAFT_CLONE_WINDOW_H

#include "core.h"

namespace MinecraftClone {
    class Window {
        private:
            GLFWwindow *glfwWindow;
            std::string title;
            bool fullscreen;
        public:
            unsigned int width, height;
            static Window *createWindow(unsigned int width, unsigned int height, const std::string &title, const bool fullscreen = false);
            static void freeWindow(Window *window);
            
            GLFWwindow *getGlfwWindow();
            void close();
            void setCallbacks();
            bool shouldClose();
            
            void setFullscreen(bool fullscreen);
            void resize(unsigned int width, unsigned int height);
            
            inline unsigned int getWidth() const {return width;}
            inline unsigned int getHeight() const {return height;}
            inline bool isFullscreen() const { return fullscreen; }
    };
}

#endif
