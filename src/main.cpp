#include "core.h"
#include "core/input.h"
#include "core/window.h"

using namespace MinecraftClone;

const int windowWidth = 720, windowHeight = 720;
const char *windowTitle = "Test Window 2: Electric Boogaloo";

float vertices[] = {
    -0.5f, 0.5f,  // Vertex 1, middle x top y
    0.5f, -0.5f, // Vertex 2, left x bottom y
    -0.5f, -0.5f // Vertex 3, right x bottom y
};

float vertices2[] = {
    -0.5f, 0.5f,  // Vertex 1, middle x top y
    0.5f, -0.5f, // Vertex 2, left x bottom y
    0.5f, 0.5f // Vertex 3, right x bottom y
};

const char *vertexSource = R"glsl(
    #version 330 core
    
    in vec2 position;
    
    void main() {
        gl_Position = vec4(position, 0.0, 2.0);
    }
)glsl";

const char *fragmentSource = R"glsl(
    #version 330 core
    
    out vec4 outColor;
    
    void main() {
        outColor = vec4(0.0, 1.0, 1.0, 1.0);
    }
)glsl";

struct Triangle {
    static void createTriangle(float *vertices) {
        // Create a virtual buffer object and bind the vertices to it
        GLuint vbo;
        glGenBuffers(1, &vbo); // Generate 1 buffer
        
        // Create and compile vertex shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexSource, nullptr);
        glCompileShader(vertexShader);
        
        // Create and compile fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
        glCompileShader(fragmentShader);
        
        // Create shader program and attach vertex and fragment shader to it
        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        
        // Bind the output of fragment shader to "outColor"
        glBindFragDataLocation(shaderProgram, 0, "outColor");
        
        // Link the shader program and use it
        glLinkProgram(shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        
        GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
        
        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

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
    
    // Create a virtual buffer object and bind the vertices to it
    GLuint vbo;
    glGenBuffers(1, &vbo); // Generate 1 buffer
    
    GLuint vbo2;
    glGenBuffers(1, &vbo2); // Generate 1 buffer
    
    // Create and compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);
    
    // Create and compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);
    
    // Create shader program and attach vertex and fragment shader to it
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    
    // Bind the output of fragment shader to "outColor"
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    
    // Link the shader program and use it
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    bool mouseAlreadyPressed = false;
    while(!window->shouldClose()) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);  // To upload data to buffer, you have to make it active with glBindBuffer
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(posAttrib);
        
        glUseProgram(shaderProgram);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo2); // To upload data to buffer, you have to make it active with glBindBuffer
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
        
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(posAttrib);
        
        glUseProgram(shaderProgram);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        bool mousePressed = Input::isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT);
        if(mousePressed && !mouseAlreadyPressed)
            printf("Click at %f, %f\n", Input::mouseX, Input::mouseY);
        mouseAlreadyPressed = mousePressed;
        
        glfwSwapBuffers(window->getGlfwWindow());
        glfwPollEvents();
    }
    
    Window::freeWindow(window);
    glfwTerminate();
}
