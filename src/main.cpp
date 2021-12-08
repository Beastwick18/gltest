#include "core.h"
#include "core/input.h"
#include "core/window.h"
#include "core/shader.h"
#include <fstream>
#include <iostream>
#include "shaders.h"

using namespace MinecraftClone;

float count = 0;
float l = 0;
const float turnintPoint = 10.0f;
const int windowWidth = 720, windowHeight = 720;
const char *windowTitle = "Test Window 2: Hold space to see something cool!";

float vertices[] = {
    -2.0f, 2.0f,  // Vertex 1, middle x top y
    2.0f, -2.0f, // Vertex 2, left x bottom y
    -2.0f, -2.0f // Vertex 3, right x bottom y
};

float vertices2[] = {
    -2.0f, 2.0f,  // Vertex 1, middle x top y
    2.0f, -2.0f, // Vertex 2, left x bottom y
    2.0f, 2.0f // Vertex 3, right x bottom y
};

float lerp(float a, float b, float t) {
    return a + t * (b-a);
}

struct Triangle {
    GLuint vbo, shaderProgram, vao;
    
    static Triangle *createTriangle(float *vertices, GLsizeiptr verticesSize, Shader *s) {
        Triangle *t = new Triangle();
        glGenBuffers(1, &t->vbo);
        
        glGenVertexArrays(1, &t->vao);
        glBindVertexArray(t->vao);
        
        glBindBuffer(GL_ARRAY_BUFFER, t->vbo);
        glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_DYNAMIC_DRAW);
        
        s->enableVertexAttribArray("position", 2, GL_FLOAT, GL_FALSE);
        return t;
    }
    
    static void freeTriangle(Triangle *t) {
        if(t != nullptr)
            delete t;
    }
    
    void render() {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
};



int main() {
    if(!glfwInit()) {
        fprintf(stderr, "Fatal: Failed to init glfw [failed in %s at line %d]\n", __FILE__, __LINE__ - 1);
        return -1;
    }
    
    Window *window = Window::createWindow(windowWidth, windowHeight, windowTitle);
    if(window == nullptr) {
        fprintf(stderr, "Fatal: Failed to create window [failed in %s at line %d]\n", __FILE__, __LINE__ - 2);
        return -1;
    }
    
    window->setCallbacks();
    
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        fprintf(stderr, "Fatal: Failed to init glad [failed in %s at line %d]\n", __FILE__, __LINE__ - 1);
        glfwTerminate();
        return -1;
    }
    // Bind the output of fragment shader to "outColor"
    Shader *s = Shader::createShader(fragmentSource, vertexSource);
    s->bindFragDataLocation(0, "outColor");
    
    Triangle *t1 = Triangle::createTriangle(vertices, sizeof(vertices), s);
    Triangle *t2 = Triangle::createTriangle(vertices2, sizeof(vertices2), s);
    
    bool mouseAlreadyPressed = false;
    bool keyAlreadyPressed = false;
    bool up = true;
    float a = 1;
    float b = 0;
    
    GLint test = s->getUniformLocation("test");
    
    double targetFramerate = 60.0;
    double timestep = 1.0/targetFramerate;
    double currentTime = glfwGetTime();
    double lastTime = currentTime;
    double deltaTime = 0;
    double dtSum = 0;
    double elapsedTime = 0;
    double tickCount = 0;
    double frameCount = 0;
    while(!window->shouldClose()) {
        
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        dtSum += deltaTime;
        elapsedTime += deltaTime;
        tickCount++;
        
        
        
        count += .5 * deltaTime * keyAlreadyPressed;
        
        l = lerp(a, b, count);
        if(count >= 1) {
            count = 0;
            a = b;
            b = 1-b;
        }
        
        bool mousePressed = Input::isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT);
        if(mousePressed && !mouseAlreadyPressed)
            printf("Click at %f, %f\n", Input::mouseX, Input::mouseY);
        mouseAlreadyPressed = mousePressed;
        
        keyAlreadyPressed = Input::isKeyDown(GLFW_KEY_SPACE);
        
        if(Input::isKeyDown(GLFW_KEY_ESCAPE))
            window->close();
        
        if(dtSum >= timestep) {
            frameCount++;
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            if(elapsedTime >= 1) {
                printf("FPS: %f, TICK: %f\n", frameCount, tickCount);
                elapsedTime = 0;
                tickCount = 0;
                frameCount = 0;
            }
            s->use();
            glUniform1f(test, l);
            t1->render();
            t2->render();
            
            glfwSwapBuffers(window->getGlfwWindow());
            dtSum = 0;
        }
        glfwPollEvents();
    }
    Triangle::freeTriangle(t1);
    Triangle::freeTriangle(t2);
    Shader::freeShader(s);
    Window::freeWindow(window);
    glfwTerminate();
}
