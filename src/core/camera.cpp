#include "core/camera.h"
#include "core/input.h"
#include "glm/gtc/matrix_transform.hpp"

using namespace MinecraftClone;

float something = 0;
Camera::Camera(Window *window, glm::vec3 startPos, float sensitivity, float cameraSpeed, float fov, float fovMin, float fovMax) {
    glfwSetInputMode(window->getGlfwWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    cameraPos = startPos;
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    
    pitch = 0;
    roll = 0;
    yaw = 0;
    
    bobbing = .15;
    
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
    
    this->window = window;
    this->sensitivity = sensitivity;
    this->cameraSpeed = cameraSpeed;
    
    this->fov = fov;
    this->fovMin = fovMin;
    this->fovMax = fovMax;
    this->noClip = false;
    
    view = glm::lookAt(cameraPos,cameraPos + cameraFront, cameraUp);
    proj = glm::perspective(glm::radians(fov), (float)window->getWidth()/(float)window->getHeight(), 0.1f, 100.0f);
}

glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float x) {
    return a + (b-a) * x;
}

float ground = 0;
float gravity = .010f;
bool jumping = false;

float lerpSpeed = 0.3f;
glm::vec3 forwardSpeed(0);
glm::vec3 sidewaysSpeed(0);
glm::vec3 verticalSpeed(0);
void Camera::update() {
    if(Input::isKeyDown(GLFW_KEY_W) == Input::isKeyDown(GLFW_KEY_S)) {
        forwardSpeed = lerp(forwardSpeed, glm::vec3(0), lerpSpeed);
    } else {
        if(Input::isKeyDown(GLFW_KEY_W)) {
            forwardSpeed = lerp(forwardSpeed, cameraSpeed * glm::normalize(cameraFront * glm::vec3(1, 0, 1)), lerpSpeed); 
        }
        if(Input::isKeyDown(GLFW_KEY_S)) {
            forwardSpeed = lerp(forwardSpeed, -cameraSpeed * glm::normalize(cameraFront * glm::vec3(1, 0, 1)), lerpSpeed);
        }
    }
    
    if(Input::isKeyDown(GLFW_KEY_A) == Input::isKeyDown(GLFW_KEY_D)) {
        sidewaysSpeed = lerp(sidewaysSpeed, glm::vec3(0), lerpSpeed);
    } else if(Input::isKeyDown(GLFW_KEY_D)) {
        sidewaysSpeed = lerp(sidewaysSpeed, glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed, lerpSpeed); 
    } else {
        sidewaysSpeed = lerp(sidewaysSpeed, glm::normalize(glm::cross(cameraFront, cameraUp)) * -cameraSpeed, lerpSpeed); 
    }
    
    if(!jumping) {
        if(Input::isKeyDown(GLFW_KEY_A) || Input::isKeyDown(GLFW_KEY_S))
            something -= .2;
        if(Input::isKeyDown(GLFW_KEY_W) || Input::isKeyDown(GLFW_KEY_D))
            something += .2;
    } 
    
    if(noClip) {
        if(Input::isKeyDown(GLFW_KEY_LEFT_SHIFT) == Input::isKeyDown(GLFW_KEY_SPACE)) {
            verticalSpeed = lerp(verticalSpeed, glm::vec3(0), lerpSpeed);
        } else if(Input::isKeyDown(GLFW_KEY_SPACE)) {
            verticalSpeed = lerp(verticalSpeed, cameraSpeed * cameraUp, lerpSpeed);
        } else {
            verticalSpeed = lerp(verticalSpeed, -cameraSpeed * cameraUp, lerpSpeed);
        }
        cameraPos += verticalSpeed + forwardSpeed + sidewaysSpeed;
    } else {
        if(Input::isKeyDown(GLFW_KEY_SPACE) && !jumping) {
            jumping = true;
            verticalSpeed = glm::vec3(0,.2,0);
        }
        verticalSpeed += glm::vec3(0,-gravity,0);
        
        cameraPos += verticalSpeed + forwardSpeed + sidewaysSpeed;
        if(cameraPos.y <= 1) {
            verticalSpeed = glm::vec3(0);
            cameraPos.y = 1;
            jumping = false;
        }
    }
    
    
    auto speed = verticalSpeed + forwardSpeed + sidewaysSpeed;
    auto newPos = cameraPos;
    if(!noClip) {
        newPos.y = bobbing*sin(something) + cameraPos.y;
    }
    view = glm::lookAt(newPos,newPos + cameraFront, cameraUp);
    proj = glm::perspective(glm::radians(fov), (float)window->getWidth()/(float)window->getHeight(), 0.1f, 100.0f);
}

glm::mat4 Camera::getProjection() {
    return proj;
}

glm::mat4 Camera::getView() {
    return view;
}

void Camera::setRotation(float xoff, float yoff) {
    yaw += xoff * sensitivity;
    pitch -= yoff * sensitivity;
    
    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;
    
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void Camera::setZoom(float yoff) {
    sensitivity += .01*yoff;
    if(sensitivity < 0)
        sensitivity = 0;
    else if(sensitivity > 10)
        sensitivity = 10;
    // fov -= yoff;
    // if(fov < fovMin)
    //     fov = fovMin;
    // else if(fov > fovMax)
    //     fov = fovMax;
}

void Camera::setFov(float fov) {
    this->fov = fov;
    if(fov < fovMin)
        fov = fovMin;
    else if(fov > fovMax)
        fov = fovMax;
}

void Camera::toggleNoclip() {
    noClip = !noClip;
    verticalSpeed = glm::vec3(0);
    forwardSpeed = glm::vec3(0);
    sidewaysSpeed = glm::vec3(0);
}
