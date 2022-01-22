#ifndef MINECRAFT_CLONE_WINDOW_H
#define MINECRAFT_CLONE_WINDOW_H

#include "core.h"

namespace MinecraftClone {
    class Window {
        private:
            GLFWwindow *glfwWindow;
            std::string title;
            bool fullscreen;
            static void error_callback(int error, const char *msg);
            unsigned int width, height;
        public:
            static Window *createWindow(unsigned int width, unsigned int height, const std::string &title, const bool fullscreen = false);
            static void freeWindow(Window *window);
            
            GLFWwindow *getGlfwWindow();
            void close();
            void setCallbacks();
            bool shouldClose();
            void swapBuffers();
            void pollEvents();
            
            void setFullscreen(bool fullscreen);
            void resize(unsigned int width, unsigned int height);
            
            inline unsigned int getWidth() const {return width;}
            inline unsigned int getHeight() const {return height;}
            inline bool isFullscreen() const { return fullscreen; }
    };
}

#endif
