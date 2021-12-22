#ifndef MINECRAFT_CLONE_CAMERA_H
#define MINECRAFT_CLONE_CAMERA_H

#include "glm/glm.hpp"
#include "core/window.h"

namespace CameraConfig {
    extern glm::vec3 cameraUp;
    extern glm::vec3 cameraFront, cameraPos;
    extern float ground, gravity;
    extern float pitch, roll, yaw;
    extern float maxPitch, minPitch;
    extern float bobbingSpeed, bobbingHeight;
    extern float jumpVelocity;
    extern float fov, fovMax, fovMin;
    extern float mouseSensitivity;
    extern float cameraSpeed, lerpSpeed;
    extern float climbSpeed;
    extern bool noclip;
    
    // Set the fov (in degrees)
    void setFov(float fov);
    
    void setRotation(float xoff, float yoff);
    void updateRotation();
    
    inline void toggleNoclip() {noclip = !noclip;}
}

class Camera {
    public:
        Camera(MinecraftClone::Window *window);
        
        void update(double deltaTime);
        glm::mat4 getView();
        glm::mat4 getProjection();
        void recalculateProjection();
        
        static void free(Camera *c);
    private:
        glm::vec3 forwardSpeed, sidewaysSpeed, verticalSpeed;
        glm::mat4 view, proj;
        float bobbing;
        bool jumping;
        MinecraftClone::Window *window;
};

#endif
