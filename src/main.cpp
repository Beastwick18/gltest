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
#include <filesystem>
#include <iostream>
#include "core/Texture2D.h"

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
    -1.0f,  0.5f, 0.0f, 0.0f, 0.0f, // Top left
     0.0f,  0.5f, 0.0f, 1.0f, 0.0f, // Top right
     0.0f, -0.5f, 0.0f, 1.0f, 1.0f, // Bottom right
    -1.0f, -0.5f, 0.0f, 0.0f, 1.0f  // Bottom left
};

float verticesSimple2[] = {
    //    Position   // Tex coords //
     0.0f,  0.5f, 0.0f, 0.0f, 0.0f, // Top left
     1.0f,  0.5f, 0.0f, 1.0f, 0.0f, // Top right
     1.0f, -0.5f, 0.0f, 1.0f, 1.0f, // Bottom right
     0.0f, -0.5f, 0.0f, 0.0f, 1.0f  // Bottom left
};

unsigned int indices[] = {
    0, 1, 2, // Top triangle
    0, 2, 3  // Bottom triangle
};

float lerp(float a, float b, float t) {
    return a + t * (b-a);
}

struct Renderable {
    VBO *vbo;
    EBO *ebo;
    VAO *vao;
    GLuint vertexPropertyCount, vertexCount, indicesCount;
    
    Renderable(GLfloat *vertices, GLuint vertexPropertyCount, GLuint vertexCount, GLuint *indices, GLuint indicesCount) {
        this->vertexPropertyCount = vertexPropertyCount;
        this->vertexCount = vertexCount;
        this->indicesCount = indicesCount;
        vao = new VAO;
        vao->bind();
        vbo = new VBO(vertices, sizeof(GLfloat) * vertexCount * vertexPropertyCount);
        ebo = new EBO(indices, indicesCount);
    }
    
    void unbindAll() const {
        vbo->unbind();
        ebo->unbind();
        vao->unbind();
    }
    
    void linkAttrib(int layout, int start, GLuint end) {
        if(end < start) {
            fprintf(stderr, "Non fatal: Invalid range for attribute [in file %s line %d]\n", __FILE__, __LINE__);
            return;
        }
        vao->bind();
        vao->linkAttrib(vbo, layout, end-start, GL_FLOAT, vertexPropertyCount * sizeof(float), (void*)(start * sizeof(float)));
        vao->unbind();
    }
    
    static void free(Renderable *r) {
        if(r != nullptr) {
            VAO::free(r->vao);
            VBO::free(r->vbo);
            EBO::free(r->ebo);
            delete r;
        }
    }
    
    void render() {
        vao->bind();
        glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
        vao->unbind();
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
        Window::freeWindow(window);
        return -1;
    }
    
    // Bind the output of fragment shader to "outColor"
    Shader *s = Shader::createShader("shaders/fragment.frag", "shaders/vertex.vert");
    s->bindFragDataLocation(0, "outColor");
    
    Renderable *r = new Renderable(verticesSimple, 5, 4, indices, 6);
    r->linkAttrib(s->getAttribLocation("position"), 0, 3); // Position
    r->linkAttrib(s->getAttribLocation("aTex"), 3, 5); // Tex coords
    r->unbindAll();
    
    Renderable *r2 = new Renderable(verticesSimple2, 5, 4, indices, 6);
    r2->linkAttrib(0, 0, 3); // Position
    r2->linkAttrib(1, 3, 5); // Tex coords
    r2->unbindAll();
    
    Texture2D *tex = Texture2D::loadFromImageFile("res/dirt.png", 0);
    Texture2D *tex2 = Texture2D::loadFromImageFile("res/anime.png", 1);
    Texture2D *tex3 = Texture2D::loadFromImageFile("res/pack.png", 0);
    
    GLuint tex0Uniform = s->getUniformLocation("tex0");
    GLuint tex1Uniform = s->getUniformLocation("tex1");
    s->use();
    glUniform1i(tex0Uniform, tex->textureIndex);
    glUniform1i(tex0Uniform, tex2->textureIndex);
    
    bool mouseAlreadyPressed = false;
    bool keyAlreadyPressed = false;
    bool playAnimation = true;
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
    int texImage = 0;
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
            texImage = !texImage;
            // playAnimation = !playAnimation;
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
            
            
            tex2->activate();
            tex2->bind();
            if(texImage == 0) {
                tex->activate();
                tex->bind();
                r->render();
                tex3->activate();
                tex3->bind();
                r2->render();
            } else {
                tex->activate();
                tex->bind();
                r2->render();
                tex3->activate();
                tex3->bind();
                r->render();
                // glUniform1i(texUniform, tex2->textureIndex);
                // r->render(tex3);
                // glUniform1i(texUniform, tex->textureIndex);
                // r2->render(tex3);
            }
            
            glfwSwapBuffers(window->getGlfwWindow());
            dtSum = 0;
        }
        glfwPollEvents();
    }
    Shader::freeShader(s);
    Window::freeWindow(window);
    Renderable::free(r);
    Renderable::free(r2);
    Texture2D::free(tex2);
    Texture2D::free(tex3);
    Texture2D::free(tex);
    glfwTerminate();
}
