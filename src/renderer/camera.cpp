#include "renderer/camera.h"
#include "input/input.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/compatibility.hpp"
#include "world/World.h"

using namespace MinecraftClone;

namespace CameraConfig {
    glm::vec3 cameraUp(0.f, 1.f, 0.f);
    glm::vec3 cameraFront(1.f, 0.f, 0.f);
    glm::vec3 cameraPos(0);
    float pitch = 0, roll = 0, yaw = 0;
    float maxPitch = 89.0f, minPitch = -89.0f;
    // float bobbingSpeed = 10.f, bobbingHeight = .15f;
    float bobbingSpeed = 10.f, bobbingHeight = 0.f;
    float jumpVelocity = 3.2f*2;
    float ground = 1.5f, gravity = 18.f;
    float fov = 90.0f, fovMax = 120.0f, fovMin = 1.0f;
    float zoomFov = 50.f;
    float mouseSensitivity = 2.5f, m_yaw = .022f;
    float cameraSpeed = 5.1f, lerpSpeed = 10.f;
    float climbSpeed = 10.f;
    int blockReach = 6;
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
    this->bobbing = 0;
    
    this->forwardSpeed = glm::vec3(0);
    this->sidewaysSpeed = glm::vec3(0);
    this->verticalSpeed = glm::vec3(0);
    
    view = glm::lookAt(CameraConfig::cameraPos,CameraConfig::cameraPos + CameraConfig::cameraFront, CameraConfig::cameraUp);
    proj = glm::perspective(glm::radians(CameraConfig::fov), (float)window->getWidth()/(float)window->getHeight(), 0.1f, 1000.0f);
}

void Camera::update(double deltaTime) {
    // Either both keys are down or neither are
    if(Input::isKeyDown(GLFW_KEY_W) == Input::isKeyDown(GLFW_KEY_S))
        forwardSpeed = glm::lerp(forwardSpeed, glm::vec3(0), CameraConfig::lerpSpeed*(float)deltaTime);
    else if(Input::isKeyDown(GLFW_KEY_W))
        forwardSpeed = glm::lerp(forwardSpeed, CameraConfig::cameraSpeed * glm::normalize(CameraConfig::cameraFront * glm::vec3(1, 0, 1)), CameraConfig::lerpSpeed*(float)deltaTime);
    else
        forwardSpeed = glm::lerp(forwardSpeed, -CameraConfig::cameraSpeed * glm::normalize(CameraConfig::cameraFront * glm::vec3(1, 0, 1)), CameraConfig::lerpSpeed*(float)deltaTime);
    
    if(Input::isKeyDown(GLFW_KEY_A) == Input::isKeyDown(GLFW_KEY_D))
        sidewaysSpeed = glm::lerp(sidewaysSpeed, glm::vec3(0), CameraConfig::lerpSpeed*(float)deltaTime);
    else if(Input::isKeyDown(GLFW_KEY_D))
        sidewaysSpeed = glm::lerp(sidewaysSpeed, glm::normalize(glm::cross(CameraConfig::cameraFront, CameraConfig::cameraUp)) * CameraConfig::cameraSpeed, CameraConfig::lerpSpeed*(float)deltaTime);
    else
        sidewaysSpeed = glm::lerp(sidewaysSpeed, glm::normalize(glm::cross(CameraConfig::cameraFront, CameraConfig::cameraUp)) * -CameraConfig::cameraSpeed, CameraConfig::lerpSpeed*(float)deltaTime);
    
    if(!jumping) {
        if(Input::isKeyDown(GLFW_KEY_W) || Input::isKeyDown(GLFW_KEY_D))
            bobbing += CameraConfig::bobbingSpeed * (float)deltaTime;
        else if(Input::isKeyDown(GLFW_KEY_A) || Input::isKeyDown(GLFW_KEY_S))
            bobbing -= CameraConfig::bobbingSpeed * (float)deltaTime;
    }
    
    if(CameraConfig::noclip) {
        if(Input::isKeyDown(GLFW_KEY_LEFT_SHIFT) == Input::isKeyDown(GLFW_KEY_SPACE))
            verticalSpeed = glm::lerp(verticalSpeed, glm::vec3(0), CameraConfig::lerpSpeed*(float)deltaTime);
        else if(Input::isKeyDown(GLFW_KEY_SPACE))
            verticalSpeed = glm::lerp(verticalSpeed, CameraConfig::cameraSpeed * CameraConfig::cameraUp, CameraConfig::lerpSpeed*(float)deltaTime);
        else
            verticalSpeed = glm::lerp(verticalSpeed, -CameraConfig::cameraSpeed * CameraConfig::cameraUp, CameraConfig::lerpSpeed*(float)deltaTime);
        
        glm::vec3 speed = verticalSpeed + forwardSpeed + sidewaysSpeed;
        CameraConfig::cameraPos += (float)(deltaTime) * (speed);
    } else {
        if(Input::isKeyDown(GLFW_KEY_SPACE) && !jumping) {
            jumping = true;
            verticalSpeed = CameraConfig::jumpVelocity * CameraConfig::cameraUp;
        }
        verticalSpeed -= (float)deltaTime * CameraConfig::gravity * CameraConfig::cameraUp;
        
        
        
        glm::vec3 speed = verticalSpeed + forwardSpeed + sidewaysSpeed;
        if(CameraConfig::cameraPos.y <= CameraConfig::ground+CameraConfig::bobbingHeight*glm::sin(bobbing)) {
            // CameraConfig::cameraPos.y = pos.y+2.7;
            verticalSpeed = glm::vec3(0);
            CameraConfig::cameraPos.y = glm::lerp(CameraConfig::cameraPos.y, CameraConfig::ground, (float)deltaTime*CameraConfig::climbSpeed);
            
            if(glm::abs(CameraConfig::cameraPos.y - CameraConfig::ground) < .01f) {
                CameraConfig::cameraPos.y = CameraConfig::ground;
            }
            jumping = false;
        }
        
        CameraConfig::cameraPos += (float) deltaTime *(speed);
    }
    
    auto newPos = CameraConfig::cameraPos;
    if(!CameraConfig::noclip) {
        newPos.y = CameraConfig::bobbingHeight*glm::sin(bobbing) + CameraConfig::cameraPos.y;
    }
    
    static float oldFov;
    static bool zoom = false;
    if(Input::isKeyBeginDown(GLFW_KEY_C)) {
        oldFov = CameraConfig::fov;
        CameraConfig::fov = CameraConfig::zoomFov;
        zoom = true;
    } else if(zoom && !Input::isKeyDown(GLFW_KEY_C)) {
        zoom = false;
        CameraConfig::fov = oldFov;
    }
    
    view = glm::lookAt(newPos,newPos + CameraConfig::cameraFront, CameraConfig::cameraUp);
    proj = glm::perspective(glm::radians(CameraConfig::fov), (float)window->getWidth()/(float)window->getHeight(), 0.1f, 1000.0f);
}

void Camera::recalculateProjection() {
}

void Camera::free(Camera *c) {
    if(c) {
        delete c;
    }
}
