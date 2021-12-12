#include "core.h"
#include "core/VBO.h"
#include "core/VAO.h"
#include "core/EBO.h"
#include "core/input.h"
#include "core/window.h"
#include "core/shader.h"
#include "shaders.h"
#include <cstring>
#include <stb/stb_image.h>
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

float verticesSimple[] = {
    //    Position   // Tex coords //
    -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, // Top left
     1.0f,  1.0f, 0.0f, 1.0f, 0.0f, // Top right
     1.0f, -1.0f, 0.0f, 1.0f, 1.0f, // Bottom right
    -1.0f, -1.0f, 0.0f, 0.0f, 1.0f  // Bottom left
};

unsigned int indices[] = {
    0, 1, 2, // Top triangle
    0, 3, 2  // Bottom triangle
};

float lerp(float a, float b, float t) {
    return a + t * (b-a);
}

// Deprecated
struct Triangle {
    GLuint vbo, vao, ebo, numVertices;
    
    static Triangle *createTriangle(float *vertices, unsigned int *indices, size_t numIndices, GLuint numVertices, GLuint numDimensions, Shader *s) {
        Triangle *t = new Triangle();
        size_t verticesSize = sizeof(float) * numVertices * numDimensions;
        size_t indicesSize = sizeof(float) * numIndices;
        t->numVertices = numVertices;
        
        glGenVertexArrays(1, &t->vao);
        glGenBuffers(1, &t->vbo);
        glGenBuffers(1, &t->ebo);
        
        glBindVertexArray(t->vao);
        
        glBindBuffer(GL_ARRAY_BUFFER, t->vbo);
        glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t->ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);
        
        s->enableVertexAttribArray("position", 2, GL_FLOAT, GL_FALSE);
        glEnableVertexAttribArray(0);
        
        // Unbind vbo and vao
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        
        return t;
    }
    
    static void freeTriangle(Triangle *t) {
        glDeleteVertexArrays(1, &t->vao);
        glDeleteBuffers(1, &t->vbo);
        if(t != nullptr)
            delete t;
    }
    
    void render() {
        glBindVertexArray(vao);
        // glDrawArrays(GL_TRIANGLES, 0, numVertices);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
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
    
    // Triangle *square = Triangle::createTriangle(verticesSimple, indices, 6, 4, 2, s);
    // Triangle *t1 = Triangle::createTriangle(vertices, sizeof(vertices), s);
    // Triangle *t2 = Triangle::createTriangle(vertices2, sizeof(vertices2), s);
    
    VAO *vao = new VAO;
    vao->bind();
    // printf("%ld, %ld", sizeof(verticesSimple), sizeof(indices));
    VBO *vbo = new VBO(verticesSimple, sizeof(verticesSimple));
    EBO *ebo = new EBO(indices, sizeof(indices));
    vao->linkAttrib(vbo, s->getAttribLocation("position"), 3, GL_FLOAT, 5 * sizeof(float), nullptr);
    vao->linkAttrib(vbo, 1, 2, GL_FLOAT, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    
    vao->unbind();
    vbo->unbind();
    ebo->unbind();
    
    int imageWidth, imageHeight, colorCh;
    unsigned char *bytes = stbi_load("unnamed.png", &imageWidth, &imageHeight, &colorCh, 0);
    printf("%d, %d\n", imageWidth, imageHeight);
    
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    stbi_image_free(bytes);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    GLuint tex0Uniform = s->getUniformLocation("tex0");
    s->use();
    glUniform1i(tex0Uniform, 0);
    
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
            glBindTexture(GL_TEXTURE_2D, texture);
            vao->bind();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            vao->unbind();
            
            // square->render();
            // t1->render();
            // t2->render();
            
            glfwSwapBuffers(window->getGlfwWindow());
            dtSum = 0;
        }
        glfwPollEvents();
    }
    // Triangle::freeTriangle(square);
    // Triangle::freeTriangle(t1);
    // Triangle::freeTriangle(t2);
    Shader::freeShader(s);
    Window::freeWindow(window);
    VBO::free(vbo);
    VAO::free(vao);
    EBO::free(ebo);
    glDeleteTextures(1, &texture);
    glfwTerminate();
}
