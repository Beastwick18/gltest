#ifndef MINECRAFT_CLONE_INPUT_H
#define MINECRAFT_CLONE_INPUT_H

#include "core.h"
#include "renderer/camera.h"

namespace MinecraftClone {
    namespace Input {
        extern bool keysPressed[GLFW_KEY_LAST];
        extern bool keysBeginPress[GLFW_KEY_LAST];
        extern bool keysBeginRelease[GLFW_KEY_LAST];
        extern bool mouseButtonsPressed[GLFW_MOUSE_BUTTON_LAST];
        extern bool mouseButtonsBeginPressed[GLFW_MOUSE_BUTTON_LAST];
        extern float mouseX, mouseY;
        extern float mouseScrollX, mouseScrollY;
        extern float deltaMouseX, deltaMouseY;
        extern Window *window;
        extern bool cursorEnabled;
        
        void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
        void mouseCallback(GLFWwindow *window, double xpos, double ypos);
        void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
        void mouseScrollCallback(GLFWwindow *window, double xoff, double yoff);
        
        void enableCursor();
        void disableCursor();
        
        bool isKeyDown(int key);
        bool isKeyBeginDown(int key);
        bool isKeyBeginUp(int key);
        bool isMouseButtonDown(int button);
        bool isMouseButtonBeginDown(int button);
        
        void reset();
    }
}

#endif
