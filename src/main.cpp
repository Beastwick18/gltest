#include <cmath>
#include <cstring>
#include <iostream>
#include "core.h"
#include "core/VBO.h"
#include "core/VAO.h"
#include "core/EBO.h"
#include "core/input.h"
#include "core/window.h"
#include "core/shader.h"
#include "core/texture2D.h"
#include "core/renderable.h"
#include "core/renderer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace MinecraftClone;

int windowWidth = 960, windowHeight = 720;
const char *windowTitle = "Test Window 2: Press space to see something cool!";
bool fullscreen = false, vsync = true;
double targetFramerate = 60.0;

// float vertices[] = {
//     -2.0f, 2.0f,  // Vertex 1, middle x top y
//     2.0f, -2.0f, // Vertex 2, left x bottom y
//     -2.0f, -2.0f // Vertex 3, right x bottom y
// };

// float vertices2[] = {
//     -2.0f, 2.0f,  // Vertex 1, middle x top y
//     2.0f, -2.0f, // Vertex 2, left x bottom y
//     2.0f, 2.0f // Vertex 3, right x bottom y
// };
// float square_vertices[] = {
//     // Triangle 1
//     -2.0f, 2.0f, // top left point
//     2.0f, -2.0f, // bottom right point
//     2.0f, 2.0f,  // top right point
    
//     // Triangle 2
//     -2.0f, 2.0f,  // top left point
//     2.0f, -2.0f, // bottom right point
//     -2.0f, -2.0f, // bottom left point
// };

float vertices[] = {
     //   Position   // Tex coords //
     0.0f,  0.5f, 1.0f, 0.0f, 1.0f, // Top left
     1.0f,  0.5f, 1.0f, 1.0f, 1.0f, // Top right
     1.0f, -0.5f, 1.0f, 1.0f, 0.0f, // Bottom right
     0.0f, -0.5f, 1.0f, 0.0f, 0.0f  // Bottom left
};

float verticesSimple2[] = {
     //   Position   // Tex coords //
     0.0f,  0.5f, 1.0f, 0.0f, 1.0f, // Top left
     1.0f,  0.5f, 1.0f, 1.0f, 1.0f, // Top right
     1.0f, -0.5f, 1.0f, 1.0f, 0.0f, // Bottom right
     0.0f, -0.5f, 1.0f, 0.0f, 0.0f  // Bottom left
};

unsigned int indices[] = {
    0, 1, 2, // Top triangle
    0, 2, 3  // Bottom triangle
};

float lerp(float a, float b, float t) {
    return a + t * (b-a);
}
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
    
    // Setup blending for textures that contain transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glEnable(GL_BLEND);
    
    // Bind the output of fragment shader to "outColor"
    Shader *s = Shader::createShader("shaders/fragment.frag", "shaders/vertex.vert");
    s->bindFragDataLocation(0, "outColor");
    
    
    // Renderable *r = new Renderable(vertices, 5, 4, indices, 6);
    // r->pushAttrib<float>(3); // Position
    // r->pushAttrib<float>(2); // Tex coords
    // r->unbindAll();
    
    VAO *vao = new VAO;
    vao->bind();
    VBO *vbo = new VBO(vertices, sizeof(vertices));
    VBlayout *layout = new VBlayout;
    layout->push<float>(3);
    layout->push<float>(2);
    vao->addBuffer(vbo, layout);
    
    EBO *ebo = new EBO(indices, 6);
    
    // Renderable *r2 = new Renderable(verticesSimple2, 5, 4, indices, 6);
    // r2->pushAttrib<float>(3); // Position
    // r2->pushAttrib<float>(2); // Tex coords
    // r2->unbindAll();
    
    Renderer renderer;
    
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window->getGlfwWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 130");
    
    Texture2D *tex = Texture2D::loadFromImageFile("res/smile.png");
    Texture2D *tex2 = Texture2D::loadFromImageFile("res/anime.png");
    Texture2D *tex3 = Texture2D::loadFromImageFile("res/pack.png");
    
    GLint test = s->getUniformLocation("test");
    GLint mvpUniform = s->getUniformLocation("MVP");
    
    // Create the projection matrix for 4:3 aspect ratio
    float projWidth = 5.0f;
    float projHeight = (projWidth * windowHeight)/windowWidth;
    glm::mat4 proj = glm::ortho(-projWidth, projWidth, -projHeight, projHeight, -1.0f, 1.0f);
    // Translate view matrix -.2f
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
    // Translate the model by .5, .5
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, .5f, 0));
    
    glm::mat4 mvp = proj * view * model;
    
    // Initialize some uniforms
    s->use();
    s->setUniformMat4f(mvpUniform, mvp);
    s->setUniform1i("tex0", 0);
    s->setUniform1i("tex1", 1);
    
    float l = 0;
    bool mouseAlreadyPressed = false;
    bool keyAlreadyPressed = false;
    bool playAnimation = true;
    bool up = true;
    float a = 1;
    float b = 0;
    float count = 0;
    int texImage = 0;
    
    double timestep = 1.0/targetFramerate;
    double currentTime = glfwGetTime();
    double lastTime = currentTime;
    double deltaTime = 0;
    double dtSum = 0;
    double elapsedTime = 0;
    double tickCount = 0;
    double frameCount = 0;
    
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    
    float moveSpeed = .1f;
    while(!window->shouldClose()) {
        glfwPollEvents();
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
            
            
            if(Input::isKeyDown(GLFW_KEY_W)) {
                view = glm::translate(view, glm::vec3(0, -moveSpeed, 0));
            }
            if(Input::isKeyDown(GLFW_KEY_A)) {
                view = glm::translate(view, glm::vec3(moveSpeed, 0, 0));
            }
            if(Input::isKeyDown(GLFW_KEY_S)) {
                view = glm::translate(view, glm::vec3(0, moveSpeed, 0));
            }
            if(Input::isKeyDown(GLFW_KEY_D)) {
                view = glm::translate(view, glm::vec3(-moveSpeed, 0, 0));
            }
            mvp = proj * view * model;
            
            s->use();
            s->setUniform1f(test, l);
            s->setUniformMat4f(mvpUniform, mvp);
            
            int start = -projWidth;
            for(int i = 0; i < 10; i++) {
                
                s->setUniformMat4f(mvpUniform, proj * model * glm::translate(view, glm::vec3(start, 0, 0)));
                renderer.render(vao, ebo, s);
                start += 1;
            }
            
            // r->render(tex);
            
            tex3->bind();
            tex2->bind(1);
            renderer.render(vao, ebo, s);
            
            // Bind tex2 to GL_TEXTURE1
            // tex2->bind(1);
            // if(texImage == 0) {
            //     r->render(tex);
            //     // Render again, but 1 unit to the right
            //     s->setUniformMat4f(mvpUniform, proj * model * glm::translate(view, glm::vec3(1.0f, 0, 0)));
            //     r->render(tex3);
            // } else {
            //     r->render(tex3);
            //     // Render again, but 1 unit to the right
            //     s->setUniformMat4f(mvpUniform, proj * model * glm::translate(view, glm::vec3(1.0f, 0, 0)));
            //     r->render(tex);
            // }
            
            
            
            dtSum = 0;
        }
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        {
            static float f = 0.0f;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        
        ImGui::Render();
        ImGui::EndFrame();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window->getGlfwWindow());
    }
    
    Shader::freeShader(s);
    Window::freeWindow(window);
    // Renderable::free(r);
    // Renderable::free(r2);
    Texture2D::free(tex);
    Texture2D::free(tex2);
    Texture2D::free(tex3);
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwTerminate();
    
    return 0;
}
