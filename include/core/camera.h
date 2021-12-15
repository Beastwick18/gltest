#ifndef MINECRAFT_CLONE_CAMERA_H
#define MINECRAFT_CLONE_CAMERA_H

#include "glm/glm.hpp"
#include "core/window.h"

class Camera {
    public:
        Camera(MinecraftClone::Window *window, glm::vec3 startPos, float sensitivity, float cameraSpeed, float fov, float fovMin, float fovMax);
        
        void update();
        glm::mat4 getView();
        glm::mat4 getProjection();
        
        void setRotation(float xoff, float yoff);
        void setZoom(float yoff);
        void setFov(float fov);
        void toggleNoclip();
        inline void setSensitivity(float sensitivity) { this->sensitivity = sensitivity; }
        inline float getFov() {return fov;}
        bool noClip = false;
        float bobbing;
    private:
        glm::vec3 cameraPos, cameraFront, cameraUp;
        glm::mat4 view, proj;
        float pitch, roll, yaw;
        float fov, sensitivity, cameraSpeed;
        float fovMax, fovMin;
        MinecraftClone::Window *window;
};

#endif
