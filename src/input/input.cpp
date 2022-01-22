#include "input/input.h"

namespace MinecraftClone {
    namespace Input {
        bool keysPressed[GLFW_KEY_LAST] = { false };
        bool keysBeginPress[GLFW_KEY_LAST] = { false };
        bool keysBeginRelease[GLFW_KEY_LAST] = { false };
        bool mouseButtonsPressed[GLFW_MOUSE_BUTTON_LAST] = { false };
        bool mouseButtonsBeginPressed[GLFW_MOUSE_BUTTON_LAST] = { false };
        float mouseX = 0.0f, mouseY = 0.0f, mouseScrollX = 0.0f, mouseScrollY = 0.0f;
        float lastMouseX = 0.0f, lastMouseY = 0.0f;
        float deltaMouseX = 0.f, deltaMouseY = 0.f;
        float sensitivity = .1f;
        bool cursorEnabled = false;
        bool first = true;
        Window *window = nullptr;
        
        void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
            if(key >= 0 && key <= GLFW_KEY_LAST) {
                if(action == GLFW_PRESS) {
                    keysPressed[key] = true;
                    keysBeginPress[key] = true;
                    keysBeginRelease[key] = false;
                } else if(action == GLFW_RELEASE) {
                    keysPressed[key] = false;
                    keysBeginPress[key] = false;
                    keysBeginRelease[key] = true;
                }
            }
        }
        
        void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
            if(button >= 0 && button < GLFW_MOUSE_BUTTON_LAST)
                mouseButtonsBeginPressed[button] = mouseButtonsPressed[button] = action == GLFW_PRESS;
        }
        
        void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
            mouseX = xpos;
            mouseY = ypos;
            if(first) {
                lastMouseX = xpos;
                lastMouseY = ypos;
                first = false;
            }
            
            if(!cursorEnabled) {
                deltaMouseX = (float)xpos - lastMouseX;
                deltaMouseY = (float)ypos - lastMouseY;
            }
            lastMouseX = xpos;
            lastMouseY = ypos;
            CameraConfig::setRotation(deltaMouseX, deltaMouseY);
        }
        
        void mouseScrollCallback(GLFWwindow *window, double xoff, double yoff) {
            mouseScrollX = (float)xoff;
            mouseScrollY = (float)yoff;
        }
        
        bool isKeyDown(int key) {
            return key >= 0 && key < GLFW_KEY_LAST && keysPressed[key];
        }
        
        bool isKeyBeginDown(int key) {
            return key >= 0 && key < GLFW_KEY_LAST && keysBeginPress[key];
        }
        
        bool isKeyBeginUp(int key) {
            return key >= 0 && key < GLFW_KEY_LAST && keysBeginRelease[key];
        }
        
        bool isMouseButtonDown(int button) {
            return button >= 0 && button < GLFW_MOUSE_BUTTON_LAST && mouseButtonsPressed[button];
        }
        
        bool isMouseButtonBeginDown(int button) {
            return button >= 0 && button < GLFW_MOUSE_BUTTON_LAST && mouseButtonsBeginPressed[button];
        }
        
        void enableCursor() {
            Input::cursorEnabled = true;
            glfwSetInputMode(window->getGlfwWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        
        void disableCursor() {
            Input::cursorEnabled = false;
            first = true;
            glfwSetInputMode(window->getGlfwWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        
        void reset() {
            deltaMouseX = 0;
            deltaMouseY = 0;
            mouseScrollX = 0;
            mouseScrollY = 0;
            std::fill(keysBeginPress, keysBeginPress + sizeof(keysBeginPress), false);
            std::fill(keysBeginRelease, keysBeginRelease + sizeof(keysBeginRelease), false);
            std::fill(mouseButtonsBeginPressed, mouseButtonsBeginPressed + sizeof(mouseButtonsBeginPressed), false);
        }
    }
}
