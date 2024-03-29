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
    bool jumping = false;
    float orthoZoom = 50.f;
    bool ortho = false;
    float guiScale = 20.f;
    float fogStart = 40.f;
    float fogEnd = 80.f;
    glm::vec3 fogColor(.85f, .85f, 1.f);
    int fogEqn = 2;
    float fogDensity = 0.015f;
    bool fogEnabled = true;
    
    void setFov(float fov) {
        CameraConfig::fov = fov;
    }
    
    void setRotation(float xoff, float yoff) {
        yaw += xoff * mouseSensitivity * m_yaw;
        if(ortho)
            pitch = -30.f;
        else
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
    this->bobbing = 0;
    
    this->forwardSpeed = glm::vec3(0);
    this->sidewaysSpeed = glm::vec3(0);
    this->verticalSpeed = glm::vec3(0);
    
    view = glm::lookAt(CameraConfig::cameraPos,CameraConfig::cameraPos + CameraConfig::cameraFront, CameraConfig::cameraUp);
    proj = glm::perspective(glm::radians(CameraConfig::fov), (float)window->getWidth()/(float)window->getHeight(), 0.1f, 1000.0f);
}

void Camera::update(double deltaTime) {
    float ftime = (float)deltaTime;
    // Either both keys are down or neither are
    if(Input::isKeyDown(GLFW_KEY_W) == Input::isKeyDown(GLFW_KEY_S))
        forwardSpeed = glm::lerp(forwardSpeed, glm::vec3(0), CameraConfig::lerpSpeed*ftime);
    else if(Input::isKeyDown(GLFW_KEY_W))
        forwardSpeed = glm::lerp(forwardSpeed, CameraConfig::cameraSpeed * glm::normalize(CameraConfig::cameraFront * glm::vec3(1, 0, 1)), CameraConfig::lerpSpeed*ftime);
    else
        forwardSpeed = glm::lerp(forwardSpeed, -CameraConfig::cameraSpeed * glm::normalize(CameraConfig::cameraFront * glm::vec3(1, 0, 1)), CameraConfig::lerpSpeed*ftime);
    
    if(Input::isKeyDown(GLFW_KEY_A) == Input::isKeyDown(GLFW_KEY_D))
        sidewaysSpeed = glm::lerp(sidewaysSpeed, glm::vec3(0), CameraConfig::lerpSpeed*ftime);
    else if(Input::isKeyDown(GLFW_KEY_D))
        sidewaysSpeed = glm::lerp(sidewaysSpeed, glm::normalize(glm::cross(CameraConfig::cameraFront, CameraConfig::cameraUp)) * CameraConfig::cameraSpeed, CameraConfig::lerpSpeed*ftime);
    else
        sidewaysSpeed = glm::lerp(sidewaysSpeed, glm::normalize(glm::cross(CameraConfig::cameraFront, CameraConfig::cameraUp)) * -CameraConfig::cameraSpeed, CameraConfig::lerpSpeed*ftime);
    
    if(!CameraConfig::jumping) {
        if(Input::isKeyDown(GLFW_KEY_W) || Input::isKeyDown(GLFW_KEY_D))
            bobbing += CameraConfig::bobbingSpeed * ftime;
        else if(Input::isKeyDown(GLFW_KEY_A) || Input::isKeyDown(GLFW_KEY_S))
            bobbing -= CameraConfig::bobbingSpeed * ftime;
    }
    
    if(CameraConfig::noclip) {
        if(Input::isKeyDown(GLFW_KEY_LEFT_SHIFT) == Input::isKeyDown(GLFW_KEY_SPACE))
            verticalSpeed = glm::lerp(verticalSpeed, glm::vec3(0), CameraConfig::lerpSpeed*ftime);
        else if(Input::isKeyDown(GLFW_KEY_SPACE))
            verticalSpeed = glm::lerp(verticalSpeed, CameraConfig::cameraSpeed * CameraConfig::cameraUp, CameraConfig::lerpSpeed*ftime);
        else
            verticalSpeed = glm::lerp(verticalSpeed, -CameraConfig::cameraSpeed * CameraConfig::cameraUp, CameraConfig::lerpSpeed*ftime);
        
        glm::vec3 speed = verticalSpeed + forwardSpeed + sidewaysSpeed;
        CameraConfig::cameraPos += ftime * speed;
    } else {
        if(Input::isKeyDown(GLFW_KEY_SPACE) && !CameraConfig::jumping) {
            CameraConfig::jumping = true;
            verticalSpeed = CameraConfig::jumpVelocity * CameraConfig::cameraUp;
        }
        verticalSpeed -= ftime * CameraConfig::gravity * CameraConfig::cameraUp;
        
        glm::vec3 speed = verticalSpeed + forwardSpeed + sidewaysSpeed;
        if(CameraConfig::cameraPos.y <= CameraConfig::ground+CameraConfig::bobbingHeight*glm::sin(bobbing)) {
            // CameraConfig::cameraPos.y = pos.y+2.7;
            verticalSpeed = glm::vec3(0);
            CameraConfig::cameraPos.y = glm::lerp(CameraConfig::cameraPos.y, CameraConfig::ground, ftime*CameraConfig::climbSpeed);
            
            if(glm::abs(CameraConfig::cameraPos.y - CameraConfig::ground) < .01f) {
                CameraConfig::cameraPos.y = CameraConfig::ground;
            }
            CameraConfig::jumping = false;
        }
        
        CameraConfig::cameraPos += ftime *(speed);
    }
    
    auto newPos = CameraConfig::cameraPos;
    if(!CameraConfig::noclip) {
        newPos.y = CameraConfig::bobbingHeight*glm::sin(bobbing) + CameraConfig::cameraPos.y;
    }
    
    if(Input::isKeyBeginDown(GLFW_KEY_C)) {
        oldFov = CameraConfig::fov;
        CameraConfig::fov = CameraConfig::zoomFov;
    } else if(Input::isKeyBeginUp(GLFW_KEY_C)) {
        CameraConfig::fov = oldFov;
    }
    
    view = glm::lookAt(newPos,newPos + CameraConfig::cameraFront, CameraConfig::cameraUp);
    if(CameraConfig::ortho) {
        float h = (float)window->getHeight() / window->getWidth();
        proj = glm::ortho(-CameraConfig::orthoZoom, CameraConfig::orthoZoom, -h * CameraConfig::orthoZoom, h * CameraConfig::orthoZoom, -1000.f, 1000.f);
    } else
        proj = glm::perspective(glm::radians(CameraConfig::fov), (float)window->getWidth()/(float)window->getHeight(), 0.1f, 1000.0f);
}

void Camera::recalculateProjection() {}

void Camera::free(Camera *c) {
    if(c) {
        delete c;
    }
}
