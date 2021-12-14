#include "OriginalScene.h"
#include "core/input.h"
#include "core/texture2D.h"
#include "core/VAO.h"
#include "core/VBO.h"
#include "core/EBO.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"

using namespace MinecraftClone;

OriginalScene::OriginalScene(Window *window) : Scene(window) {
    this->window = window;
    // Bind the output of fragment shader to "outColor"
    s = Shader::createShader("shaders/fragment.frag", "shaders/vertex.vert");
    s->bindFragDataLocation(0, "outColor");
    
    
    // Renderable *r = new Renderable(vertices, 5, 4, indices, 6);
    // r->pushAttrib<float>(3); // Position
    // r->pushAttrib<float>(2); // Tex coords
    // r->unbindAll();
    
    float vertices[] = {
         //   Position   // Tex coords //
         0.0f,  0.5f, 1.0f, 0.0f, 1.0f, // Top left
         1.0f,  0.5f, 1.0f, 1.0f, 1.0f, // Top right
         1.0f, -0.5f, 1.0f, 1.0f, 0.0f, // Bottom right
         0.0f, -0.5f, 1.0f, 0.0f, 0.0f, // Bottom left
        
         0.0f,  0.5f, 0.0f, 1.0f, 1.0f, // Top left back
         1.0f,  0.5f, 0.0f, 0.0f, 1.0f, // Top right back
        
         1.0f, -0.5f, 0.0f, 0.0f, 0.0f, // Bottom right back
         0.0f, -0.5f, 0.0f, 1.0f, 0.0f, // Bottom left back
        
         1.0f, -0.5f, 0.0f, 1.0f, 1.0f, // Bottom right back
         0.0f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom left back
        
         0.0f,  0.5f, 0.0f, 0.0f, 0.0f, // Top left back
         1.0f,  0.5f, 0.0f, 1.0f, 0.0f, // Top right back
    };
    
    unsigned int indices[] = {
        // Front face
        0, 1, 2,
        0, 2, 3,
        
        // Top face
        0, 1, 10,
        1, 10, 11,
        
        // Left face
        0, 4, 7,
        0, 3, 7,
        
        // Right face
        1, 5, 6,
        1, 2, 6,
        
        // Bottom face
        2, 3, 8,
        3, 9, 8,
        
        // Back face
        4, 5, 6,
        4, 6, 7,
    };
    
    vao = new VAO;
    vao->bind();
    vbo = new VBO(vertices, sizeof(vertices));
    VBlayout *layout = new VBlayout;
    layout->push<float>(3);
    layout->push<float>(2);
    vao->addBuffer(vbo, layout);
    delete layout;
    
    ebo = new EBO(indices, 36);
    
    // Renderable *r2 = new Renderable(verticesSimple2, 5, 4, indices, 6);
    // r2->pushAttrib<float>(3); // Position
    // r2->pushAttrib<float>(2); // Tex coords
    // r2->unbindAll();
    
    tex = Texture2D::loadFromImageFile("res/smile.png");
    tex2 = Texture2D::loadFromImageFile("res/anime.png");
    tex3 = Texture2D::loadFromImageFile("res/dirt.png");
    
    test = s->getUniformLocation("test");
    mvpUniform = s->getUniformLocation("MVP");
    
    // Create the projection matrix for 4:3 aspect ratio
    projWidth = 5.0f;
    float projHeight = (projWidth * window->getHeight())/window->getWidth();
    proj = glm::ortho(-projWidth, projWidth, -projHeight, projHeight, -1.0f, 1.0f);
    // Translate view matrix -.2f
    view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
    // Translate the model by .5, .5
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
    
    mvp = proj * view * model;
    
    // Initialize some uniforms
    s->use();
    s->setUniformMat4f(mvpUniform, mvp);
    s->setUniform1i("tex0", 0);
    // s->setUniform1i("tex1", 1);
    
    l = 0;
    mouseAlreadyPressed = false;
    keyAlreadyPressed = false;
    playAnimation = true;
    count = 0;
    texImage = 0;
    moveSpeed = .1f;
    camX = 0;
    camY = 5;
    camZ = -5;
    rotX = 0;
    rotY = 0;
}
glm::mat4 rot(-1.0f, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1);
OriginalScene::~OriginalScene() {
    Shader::freeShader(s);
    VBO::free(vbo);
    VAO::free(vao);
    EBO::free(ebo);
    Texture2D::free(tex);
    Texture2D::free(tex2);
    Texture2D::free(tex3);
}

float lookX = 0, lookY = 0;
void OriginalScene::update() {
    if(playAnimation)
        count += .01f;
    
    l = std::abs(std::sin(count));
    // reset once over 2pi
    if(count > 2*3.14f) count = 0;
    
    if(Input::isKeyDown(GLFW_KEY_W)) {
        camZ += moveSpeed;
    }
    if(Input::isKeyDown(GLFW_KEY_A)) {
        camX += moveSpeed;
    }
    if(Input::isKeyDown(GLFW_KEY_S)) {
        camZ -= moveSpeed;
    }
    if(Input::isKeyDown(GLFW_KEY_D)) {
        camX -= moveSpeed;
    }
    if(Input::isKeyDown(GLFW_KEY_LEFT_SHIFT))
        camY -= moveSpeed;
    if(Input::isKeyDown(GLFW_KEY_SPACE))
        camY += moveSpeed;
    if(Input::isKeyDown(GLFW_KEY_DOWN)) {
        lookY -= moveSpeed;
    }
    if(Input::isKeyDown(GLFW_KEY_UP)) {
        lookY += moveSpeed;
    }
    if(Input::isKeyDown(GLFW_KEY_LEFT)) {
        lookX += moveSpeed;
    }
    if(Input::isKeyDown(GLFW_KEY_RIGHT)) {
        lookX -= moveSpeed;
    }
    // camX = sin(glfwGetTime()) * 10.0f;
    // camZ = cos(glfwGetTime()) * 10.0f;
    projWidth = 5.0f;
    glm::vec3 look(lookX,lookY,0);
    glm::vec3 cameraPos(camX,camY,camZ);
    view = glm::lookAt(cameraPos,look, glm::vec3(0.0f,1.0f,0.0f));
    float projHeight = (projWidth * window->getHeight())/window->getWidth();
    // proj = glm::ortho(-projWidth, projWidth, -projHeight, projHeight, -1.0f, 100.0f);
    proj = glm::perspective(glm::radians(45.0f), (float)window->getWidth()/(float)window->getHeight(), 0.1f, 100.0f);
    mvp = proj * view * model;
    
    bool mousePressed = Input::isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT);
    if(mousePressed && !mouseAlreadyPressed)
        printf("Click at %f, %f\n", Input::mouseX, Input::mouseY);
    mouseAlreadyPressed = mousePressed;
    
    bool keyPressed = Input::isKeyDown(GLFW_KEY_SPACE);
    if(keyPressed && !keyAlreadyPressed)
        texImage = !texImage;
    keyAlreadyPressed = keyPressed;
}

void OriginalScene::render(const Renderer &renderer) {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
    
    s->use();
    s->setUniform1f(test, l);
    // s->setUniformMat4f(mvpUniform, mvp);
    int start = -projWidth;
    tex3->bind();
    float time = glfwGetTime();
    for(int i = 0; i < 10; i++) {
        float yoff = sin(time*3 + 0.5f*(float)i);
        s->setUniformMat4f(mvpUniform, proj * view * glm::translate(model, glm::vec3(start*1.1f, yoff, 0.0f)));
        renderer.render(vao, ebo, s);
        start += 1;
    }
    
    // r->render(tex);
    
    // tex2->bind(1);
    // tex3->bind();
    // renderer.render(vao, ebo, s);
    
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
}

void OriginalScene::guiRender() {
    
    static float f = 0.0f;

    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}
