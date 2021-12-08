#include "core.h"
#include "core/input.h"
#include "core/window.h"
#include <fstream>
#include <iostream>
#include "shaders.h"

using namespace MinecraftClone;

GLint test;
float count = 0;
float l = 0;
const float turnintPoint = 200.0f;
const int windowWidth = 720, windowHeight = 720;
const char *windowTitle = "Test Window 2: Electric Boogaloo";

float vertices[] = {
    -1.0f, 2.0f,  // Vertex 1, middle x top y
    1.0f, -2.0f, // Vertex 2, left x bottom y
    -1.0f, -2.0f // Vertex 3, right x bottom y
};

float vertices2[] = {
    -1.0f, 2.0f,  // Vertex 1, middle x top y
    1.0f, -2.0f, // Vertex 2, left x bottom y
    1.0f, 2.0f // Vertex 3, right x bottom y
};

float lerp(float a, float b, float t) {
    return a + t * (b-a);
}

struct Shader {
    GLuint program;
    
    static Shader *createShader(const char *fragSource, const char *vertSource) {
        Shader *s = new Shader;
        
        GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertShader, 1, &vertSource, nullptr);
        glCompileShader(vertShader);
        
        GLint result;
        int logLength;
        glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
        if(result == GL_FALSE) {
            glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<GLchar> vertShaderError((logLength > 1) ? logLength : 1);
            glGetShaderInfoLog(vertShader, logLength, NULL, &vertShaderError[0]);
            std::cout << &vertShaderError[0] << std::endl;
        }
        
        // Create and compile fragment shader
        GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader, 1, &fragSource, nullptr);
        glCompileShader(fragShader);
        
        glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
        if(result == GL_FALSE) {
            glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<GLchar> fragShaderError( (logLength > 1) ? logLength : 1 );
            glGetShaderInfoLog(fragShader, logLength, NULL, &fragShaderError[0]);
            std::cout << &fragShaderError[0] << std::endl;
        }
        
        // Create shader program and attach vertex and fragment shader to it
        s->program = glCreateProgram();
        glAttachShader(s->program, vertShader);
        glAttachShader(s->program, fragShader);
        
        glLinkProgram(s->program);
        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
        
        return s;
    }
    
    static void freeShader(Shader *s) {
        if(s != nullptr)
            delete s;
    }
};

struct Triangle {
    GLuint vbo, shaderProgram, vao;
    
    static Triangle *createTriangle(float *vertices, GLsizeiptr verticesSize, Shader *s) {
        Triangle *t = new Triangle();
        glGenBuffers(1, &t->vbo);
        
        t->shaderProgram = s->program;
        glGenVertexArrays(1, &t->vao);
        glBindVertexArray(t->vao);
        
        glBindBuffer(GL_ARRAY_BUFFER, t->vbo);
        glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_DYNAMIC_DRAW);
        GLint posAttrib = glGetAttribLocation(t->shaderProgram, "position");
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(posAttrib);
        
        // GLint fsPosAttrib = glad_glGetAttribLocation(t->shaderProgram, "fs_position");
        // glVertexAttribPointer(fsPosAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
        // glEnableVertexAttribArray(fsPosAttrib);
        return t;
    }
    
    static void freeTriangle(Triangle *t) {
        if(t != nullptr)
            delete t;
    }
    
    void render() {
        glUseProgram(shaderProgram);
        glUniform1f(test, l);
        
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
    glBindFragDataLocation(s->program, 0, "outColor");
    
    Triangle *t1 = Triangle::createTriangle(vertices, sizeof(vertices), s);
    Triangle *t2 = Triangle::createTriangle(vertices2, sizeof(vertices2), s);
    
    bool mouseAlreadyPressed = false;
    bool keyAlreadyPressed = false;
    bool up = true;
    float a = 1;
    float b = 0;
    test = glGetUniformLocation(s->program, "test");
    while(!window->shouldClose()) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        count += (1/turnintPoint);
        
        l = lerp(a, b, count);
        if(count >= 1) {
            count = 0;
            a = 1-a;
            b = 1-b;
        }
        t1->render();
        t2->render();
        
        bool mousePressed = Input::isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT);
        if(mousePressed && !mouseAlreadyPressed)
            printf("Click at %f, %f\n", Input::mouseX, Input::mouseY);
        mouseAlreadyPressed = mousePressed;
        
        bool keyPressed = Input::isKeyDown(GLFW_KEY_ESCAPE);
        if(keyPressed && !keyAlreadyPressed)
            window->close();
        keyAlreadyPressed = keyPressed;
        
        glfwSwapBuffers(window->getGlfwWindow());
        glfwPollEvents();
    }
    Triangle::freeTriangle(t1);
    Triangle::freeTriangle(t2);
    Shader::freeShader(s);
    Window::freeWindow(window);
    glfwTerminate();
}
