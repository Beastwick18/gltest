#ifndef MINECRAFT_CLONE_INPUT_H
#define MINECRAFT_CLONE_INPUT_H

#include "core.h"
#include "core/camera.h"

namespace MinecraftClone {
    namespace Input {
        extern bool keysPressed[GLFW_KEY_LAST];
        extern bool mouseButtonsPressed[GLFW_MOUSE_BUTTON_LAST];
        extern float mouseX, mouseY, mouseScrollX, mouseScrollY;
        extern Camera *camera;
        extern Window *window;
        extern bool cursorEnabled;
        
        void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
        void mouseCallback(GLFWwindow *window, double xpos, double ypos);
        void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
        void mouseScrollCallback(GLFWwindow *window, double xoff, double yoff);
        
        void installCamera(Camera *camera);
        void uninstallCamera();
        
        void enableCursor();
        void disableCursor();
        
        bool isKeyDown(int key);
        bool isMouseButtonDown(int button);
    }
}

#endif
