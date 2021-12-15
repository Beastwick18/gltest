#include "core/input.h"

namespace MinecraftClone {
    namespace Input {
        bool keysPressed[GLFW_KEY_LAST] = { false };
        bool mouseButtonsPressed[GLFW_MOUSE_BUTTON_LAST] = { false };
        float mouseX = 0.0f, mouseY = 0.0f, mouseScrollX = 0.0f, mouseScrollY = 0.0f;
        float lastMouseX = 0.0f, lastMouseY = 0.0f;
        float sensitivity = .1f;
        bool cursorEnabled = false;
        bool first = true;
        
        Camera *camera = nullptr;
        Window *window = nullptr;
        
        void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
            if(key >= 0 && key < GLFW_KEY_LAST)
                keysPressed[key] = action == GLFW_PRESS || action == GLFW_REPEAT;
        }
        
        void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
            if(button >= 0 && button < GLFW_MOUSE_BUTTON_LAST)
                mouseButtonsPressed[button] = action == GLFW_PRESS;
        }
        
        void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
            float xoffset = xpos - lastMouseX;
            float yoffset = ypos - lastMouseY;
            lastMouseX = xpos;
            lastMouseY = ypos;
            
            if(camera != nullptr && !first && !cursorEnabled) {
                camera->setRotation(xoffset, yoffset);
            }
            first = false;
        }
        
        void mouseScrollCallback(GLFWwindow *window, double xoff, double yoff) {
            mouseScrollX = (float)xoff;
            mouseScrollY = (float)yoff;
            
            if(camera != nullptr) {
                camera->setZoom(yoff);
            }
        }
        
        bool isKeyDown(int key) {
            return key >= 0 && key < GLFW_KEY_LAST && keysPressed[key];
        }
        
        bool isMouseButtonDown(int button) {
            return button >= 0 && button < GLFW_MOUSE_BUTTON_LAST && mouseButtonsPressed[button];
        }
        
        void installCamera(Camera *camera) {
            if(Input::camera == nullptr)
                Input::camera = camera;
        }
        
        void uninstallCamera() {
            Input::camera = nullptr;
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
    }
}
