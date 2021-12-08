#include "core/input.h"

namespace MinecraftClone {
    namespace Input {
        bool keysPressed[GLFW_KEY_LAST] = { false };
        bool mouseButtonsPressed[GLFW_MOUSE_BUTTON_LAST] = { false };
        float mouseX = 0.0f, mouseY = 0.0f, mouseScrollX = 0.0f, mouseScrollY = 0.0f;
        
        void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
            if(key >= 0 && key < GLFW_KEY_LAST)
                keysPressed[key] = action == GLFW_PRESS;
        }
        
        void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
            if(button >= 0 && button < GLFW_MOUSE_BUTTON_LAST)
                mouseButtonsPressed[button] = action == GLFW_PRESS;
        }
        
        void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
            mouseX = (float)xpos;
            mouseY = (float)ypos;
        }
        
        void mouseScrollCallback(GLFWwindow *window, double xoff, double yoff) {
            mouseScrollX = (float)xoff;
            mouseScrollY = (float)yoff;
        }
        
        bool isKeyDown(int key) {
            return key >= 0 && key < GLFW_KEY_LAST && keysPressed[key];
        }
        
        bool isMouseButtonDown(int button) {
            return button >= 0 && button < GLFW_MOUSE_BUTTON_LAST && mouseButtonsPressed[button];
        }
    }
}
