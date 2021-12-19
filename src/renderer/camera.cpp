#include "renderer/camera.h"
#include "input/input.h"
#include "glm/gtc/matrix_transform.hpp"

using namespace MinecraftClone;

namespace CameraConfig {
    glm::vec3 cameraUp(0.f, 1.f, 0.f);
    glm::vec3 cameraFront(1.f, 0.f, 0.f);
    glm::vec3 cameraPos(0);
    float pitch = 0, roll = 0, yaw = 0;
    float maxPitch = 89.0f, minPitch = -89.0f;
    float bobbingSpeed = 10.f, bobbingHeight = .15f;
    float jumpVelocity = 3.2f*2;
    float ground = 1.5f, gravity = 18.f;
    float fov = 90.0f, fovMax = 120.0f, fovMin = 1.0f;
    float mouseSensitivity = 2.5f, m_yaw = .022f;
    float cameraSpeed = 5.1f, lerpSpeed = 10.f;
    bool noclip = false;
    
    void setFov(float fov) {
        CameraConfig::fov = fov;
    }
    
    void setRotation(float xoff, float yoff) {
        yaw += xoff * mouseSensitivity * m_yaw;
        pitch -= yoff * mouseSensitivity * m_yaw;
        
        if(pitch > maxPitch)
            pitch = maxPitch;
        if(pitch < minPitch)
            pitch = minPitch;
        
        yaw = yaw - floor(yaw / 360.f) * 360.f;
        
        updateRotation();
    }
    
    void updateRotation() {
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);
    }
}

Camera::Camera(Window *window) {
    Input::disableCursor();
    
    CameraConfig::updateRotation();
    
    this->window = window;
    this->jumping = false;
    
    this->forwardSpeed = glm::vec3(0);
    this->sidewaysSpeed = glm::vec3(0);
    this->verticalSpeed = glm::vec3(0);
    
    view = glm::lookAt(CameraConfig::cameraPos,CameraConfig::cameraPos + CameraConfig::cameraFront, CameraConfig::cameraUp);
    proj = glm::perspective(glm::radians(CameraConfig::fov), (float)window->getWidth()/(float)window->getHeight(), 0.1f, 100.0f);
}

glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float x) {
    return a + (b-a) * x;
}

void Camera::update(double deltaTime) {
    // CameraConfig::setRotation(Input::deltaMouseX, Input::deltaMouseY);
    
    // Either both keys are down or neither are
    if(Input::isKeyDown(GLFW_KEY_W) == Input::isKeyDown(GLFW_KEY_S)) {
        forwardSpeed = lerp(forwardSpeed, glm::vec3(0), CameraConfig::lerpSpeed*deltaTime);
    } else {
        if(Input::isKeyDown(GLFW_KEY_W))
            forwardSpeed = lerp(forwardSpeed, CameraConfig::cameraSpeed * glm::normalize(CameraConfig::cameraFront * glm::vec3(1, 0, 1)), CameraConfig::lerpSpeed*deltaTime);
        if(Input::isKeyDown(GLFW_KEY_S))
            forwardSpeed = lerp(forwardSpeed, -CameraConfig::cameraSpeed * glm::normalize(CameraConfig::cameraFront * glm::vec3(1, 0, 1)), CameraConfig::lerpSpeed*deltaTime);
    }
    
    if(Input::isKeyDown(GLFW_KEY_A) == Input::isKeyDown(GLFW_KEY_D))
        sidewaysSpeed = lerp(sidewaysSpeed, glm::vec3(0), CameraConfig::lerpSpeed*deltaTime);
    else if(Input::isKeyDown(GLFW_KEY_D))
        sidewaysSpeed = lerp(sidewaysSpeed, glm::normalize(glm::cross(CameraConfig::cameraFront, CameraConfig::cameraUp)) * CameraConfig::cameraSpeed, CameraConfig::lerpSpeed*deltaTime);
    else
        sidewaysSpeed = lerp(sidewaysSpeed, glm::normalize(glm::cross(CameraConfig::cameraFront, CameraConfig::cameraUp)) * -CameraConfig::cameraSpeed, CameraConfig::lerpSpeed*deltaTime);
    
    if(!jumping) {
        if(Input::isKeyDown(GLFW_KEY_W) || Input::isKeyDown(GLFW_KEY_D))
            bobbing += CameraConfig::bobbingSpeed * deltaTime;
        else if(Input::isKeyDown(GLFW_KEY_A)  || Input::isKeyDown(GLFW_KEY_S))
            bobbing -= CameraConfig::bobbingSpeed * deltaTime;
    }
    
    if(CameraConfig::noclip) {
        if(Input::isKeyDown(GLFW_KEY_LEFT_SHIFT) == Input::isKeyDown(GLFW_KEY_SPACE))
            verticalSpeed = lerp(verticalSpeed, glm::vec3(0), CameraConfig::lerpSpeed*deltaTime);
        else if(Input::isKeyDown(GLFW_KEY_SPACE))
            verticalSpeed = lerp(verticalSpeed, CameraConfig::cameraSpeed * CameraConfig::cameraUp, CameraConfig::lerpSpeed*deltaTime);
        else
            verticalSpeed = lerp(verticalSpeed, -CameraConfig::cameraSpeed * CameraConfig::cameraUp, CameraConfig::lerpSpeed*deltaTime);
        CameraConfig::cameraPos += (float)(deltaTime) * (verticalSpeed + forwardSpeed + sidewaysSpeed);
    } else {
        if(Input::isKeyDown(GLFW_KEY_SPACE) && !jumping) {
            jumping = true;
            verticalSpeed = CameraConfig::jumpVelocity * CameraConfig::cameraUp;
        }
        verticalSpeed -= (float)deltaTime * CameraConfig::gravity * CameraConfig::cameraUp;
        
        CameraConfig::cameraPos += (float) deltaTime *(verticalSpeed + forwardSpeed + sidewaysSpeed);
        if(CameraConfig::cameraPos.y <= CameraConfig::ground) {
            // verticalSpeed = -verticalSpeed * .9f;
            verticalSpeed = glm::vec3(0);
            CameraConfig::cameraPos.y = CameraConfig::ground;
            jumping = false;
        }
    }
    
    // auto speed = glm::normalize(forwardSpeed + sidewaysSpeed) + verticalSpeed;
    auto newPos = CameraConfig::cameraPos;
    if(!CameraConfig::noclip) {
        newPos.y = CameraConfig::bobbingHeight*sin(bobbing) + CameraConfig::cameraPos.y;
    }
    
    view = glm::lookAt(newPos,newPos + CameraConfig::cameraFront, CameraConfig::cameraUp);
    proj = glm::perspective(glm::radians(CameraConfig::fov), (float)window->getWidth()/(float)window->getHeight(), 0.1f, 100.0f);
}

glm::mat4 Camera::getProjection() {
    return proj;
}

glm::mat4 Camera::getView() {
    return view;
}

void Camera::free(Camera *c) {
    if(c) {
        delete c;
    }
}

// void Camera::setRotation(float xoff, float yoff) {
//     CameraConfig::yaw += xoff * CameraConfig::sensitivity;
//     CameraConfig::pitch -= yoff * sensitivity;
    
//     if(pitch > 89.0f)
//         pitch = 89.0f;
//     if(pitch < -89.0f)
//         pitch = -89.0f;
    
//     glm::vec3 direction;
//     direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
//     direction.y = sin(glm::radians(pitch));
//     direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
//     cameraFront = glm::normalize(direction);
// }

// void Camera::setZoom(float yoff) {
//     sensitivity += .01*yoff;
//     if(sensitivity < 0)
//         sensitivity = 0;
//     else if(sensitivity > 10)
//         sensitivity = 10;
// }

// Set fov (in degrees)
// void Camera::setFov(float fov) {
//     this->fov = glm::radians(fov);
//     if(fov < fovMin)
//         fov = fovMin;
//     else if(fov > fovMax)
//         fov = fovMax;
// }

// void Camera::toggleNoclip() {
//     noClip = !noClip;
//     verticalSpeed = glm::vec3(0);
//     forwardSpeed = glm::vec3(0);
//     sidewaysSpeed = glm::vec3(0);
// }
