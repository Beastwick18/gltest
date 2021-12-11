#include "core.h"
#include "core/input.h"
#include "core/window.h"
#include "core/shader.h"
#include "shaders.h"
#include <cstring>
#include <cmath>

using namespace MinecraftClone;

float count = 0;
float l = 0;
const float turnintPoint = 10.0f;
int windowWidth = 720, windowHeight = 720;
const char *windowTitle = "Test Window 2: Press space to see something cool!";
bool fullscreen = false, vsync = true;
double targetFramerate = 60.0;

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
float square_vertices[] = {
    // Triangle 1
    -2.0f, 2.0f, // top left point
    2.0f, -2.0f, // bottom right point
    2.0f, 2.0f,  // top right point
    
    // Triangle 2
    -2.0f, 2.0f,  // top left point
    2.0f, -2.0f, // bottom right point
    -2.0f, -2.0f, // bottom left point
};

float lerp(float a, float b, float t) {
    return a + t * (b-a);
}

struct Triangle {
    GLuint vbo, vao, numVertices;
    
    static Triangle *createTriangle(float *vertices, GLuint numVertices, GLuint numDimensions, Shader *s) {
        Triangle *t = new Triangle();
        glGenBuffers(1, &t->vbo);
        size_t verticesSize = sizeof(float) * numVertices * numDimensions;
        t->numVertices = numVertices;
        
        glBindBuffer(GL_ARRAY_BUFFER, t->vbo);
        glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_DYNAMIC_DRAW);
        
        glGenVertexArrays(1, &t->vao);
        glBindVertexArray(t->vao);
        s->enableVertexAttribArray("position", 2, GL_FLOAT, GL_FALSE);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        
        return t;
    }
    
    static void freeTriangle(Triangle *t) {
        if(t != nullptr)
            delete t;
    }
    
    void render() {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
    }
};

void readArguments(int argc, char **argv) {
    for(int i = 0; i < argc; i++) {
        char *str = argv[i];
        size_t len = strlen(str);
        if(len >= 3 && str[0] == '-' && str[1] == '-') {
            char *commandString = &str[2]; // Remove "--"
            if(std::strcmp(commandString, "fullscreen") == 0) {
                fullscreen = true;
            } else if(std::strcmp(commandString, "windowed") == 0) {
                fullscreen = false;
            } else if(std::strcmp(commandString, "novsync") == 0) {
                vsync = false;
            } else if(std::strcmp(commandString, "vsync") == 0) {
                vsync = true;
            } else { // Check for "--command=value" commands
                size_t commandLen = strlen(commandString);
                bool foundValue = false;
                char *value;
                for(int j = 0; j < commandLen; j++) {
                    if(commandString[j] == '=') {
                        foundValue = true;
                        commandString[j] = 0;
                        value = &commandString[j+1];
                        break;
                    }
                }
                if(!foundValue) {
                    printf("Not recognized: %s\n", commandString);
                    continue;
                }
                if(std::strcmp(commandString, "width") == 0) {
                    windowWidth = atoi(value);
                } else if(std::strcmp(commandString, "height") == 0) {
                    windowHeight = atoi(value);
                } else if(std::strcmp(commandString, "fps") == 0) {
                    targetFramerate = atoi(value);
                } else {
                    printf("Not recognized: %s\n", commandString);
                }
            }
        }
    }
}

int main(int argc, char **argv) {
    if(!glfwInit()) {
        fprintf(stderr, "Fatal: Failed to init glfw [failed in %s at line %d]\n", __FILE__, __LINE__ - 1);
        return -1;
    }
    readArguments(argc, argv);
    
    Window *window = Window::createWindow(windowWidth, windowHeight, windowTitle, fullscreen);
    if(window == nullptr) {
        fprintf(stderr, "Fatal: Failed to create window [failed in %s at line %d]\n", __FILE__, __LINE__ - 2);
        return -1;
    }
    glfwSwapInterval(vsync);
    
    window->setCallbacks();
    
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        fprintf(stderr, "Fatal: Failed to init glad [failed in %s at line %d]\n", __FILE__, __LINE__ - 1);
        glfwTerminate();
        return -1;
    }
    // Bind the output of fragment shader to "outColor"
    Shader *s = Shader::createShader(fragmentSource, vertexSource);
    s->bindFragDataLocation(0, "outColor");
    
    Triangle *square = Triangle::createTriangle(square_vertices, 6, 2, s);
    // Triangle *t1 = Triangle::createTriangle(vertices, sizeof(vertices), s);
    // Triangle *t2 = Triangle::createTriangle(vertices2, sizeof(vertices2), s);
    
    bool mouseAlreadyPressed = false;
    bool keyAlreadyPressed = false;
    bool playAnimation = false;
    bool up = true;
    float a = 1;
    float b = 0;
    
    GLint test = s->getUniformLocation("test");
    
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
        
        if(playAnimation)
            count += .5 * deltaTime;
        
        l = std::abs(std::sin(count));
        // reset once over 2pi
        if(count > 2*3.14f) count = 0;
        
        bool mousePressed = Input::isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT);
        if(mousePressed && !mouseAlreadyPressed)
            printf("Click at %f, %f\n", Input::mouseX, Input::mouseY);
        mouseAlreadyPressed = mousePressed;
        
        bool keyPressed = Input::isKeyDown(GLFW_KEY_SPACE);
        if(keyPressed && !keyAlreadyPressed)
            playAnimation = !playAnimation;
        keyAlreadyPressed = keyPressed;
        
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
            square->render();
            // t1->render();
            // t2->render();
            
            glfwSwapBuffers(window->getGlfwWindow());
            dtSum = 0;
        }
        glfwPollEvents();
    }
    Triangle::freeTriangle(square);
    // Triangle::freeTriangle(t1);
    // Triangle::freeTriangle(t2);
    Shader::freeShader(s);
    Window::freeWindow(window);
    glfwTerminate();
}
