#include "core/shader.h"
#include <fstream>

Shader *Shader::createShader(std::string fragFilePath, std::string vertFilePath) {
    Shader *s = new Shader;
    
    GLint result;
    int logLength;
    
    // Create and compile vertex shader
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    std::string vertString = readFile(vertFilePath);
    const char *vertSource = vertString.c_str();
    
    glShaderSource(vertShader, 1, &vertSource, nullptr);
    glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE) {
        glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> vertShaderError((logLength > 1) ? logLength : 1);
        glGetShaderInfoLog(vertShader, logLength, NULL, &vertShaderError[0]);
        fprintf(stderr, "Non fatal: %s\n", &vertShaderError[0]);
    }
    
    // Create and compile fragment shader
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragString = readFile(fragFilePath).c_str();
    const char *fragSource = fragString.c_str();
    glShaderSource(fragShader, 1, &fragSource, nullptr);
    glCompileShader(fragShader);
    
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE) {
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> fragShaderError( (logLength > 1) ? logLength : 1 );
        glGetShaderInfoLog(fragShader, logLength, NULL, &fragShaderError[0]);
        fprintf(stderr, "%s\n", &fragShaderError[0]);
    }
    
    // Create shader program and attach vertex and fragment shader to it
    s->shaderProgram = glCreateProgram();
    glAttachShader(s->shaderProgram, vertShader);
    glAttachShader(s->shaderProgram, fragShader);
    
    glLinkProgram(s->shaderProgram);
    // Vertex and fragment are already attached to program, so we can delete them.
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    
    return s;
}

std::string Shader::readFile(std::string filepath) {
    std::ifstream in(filepath);
    std::string line;
    std::string out;
    while(std::getline(in, line)) {
        out += line + "\n";
    }
    in.close();
    return out;
}

GLint Shader::getUniformLocation(const char *name) {
    return glGetUniformLocation(shaderProgram, name);
}

void Shader::freeShader(Shader *s) {
    if(s != nullptr) {
        glDeleteShader(s->shaderProgram);
        delete s;
    }
}

void Shader::use() {
    glUseProgram(shaderProgram);
}

void Shader::enableVertexAttribArray(const char *name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) {
    GLint attrib = glGetAttribLocation(shaderProgram, name);
    glVertexAttribPointer(attrib, size, type, normalized, stride, pointer);
    glEnableVertexAttribArray(attrib);
}

GLint Shader::getAttribLocation(const char *name) {
    return glGetAttribLocation(shaderProgram, name);
}

void Shader::bindFragDataLocation(GLuint colorNumber, const char *name) {
    glBindFragDataLocation(shaderProgram, colorNumber, name);
}
