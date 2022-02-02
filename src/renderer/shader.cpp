#include "renderer/shader.h"
#include "glm/gtc/type_ptr.hpp"
#include <fstream>
#include <optional>

Shader *Shader::createShader(const char *shaderPath) {
    Shader *s = new Shader;
    
    const auto source = readFile(shaderPath);
    if(!source)
        return nullptr;
    const auto [vertexSource, fragmentSource] = *source;
    
    // Create and compile vertex and fragment shader
    auto vertShader = compileShader(vertexSource.c_str(), GL_VERTEX_SHADER);
    auto fragShader = compileShader(fragmentSource.c_str(), GL_FRAGMENT_SHADER);
    
    if(!vertShader || !fragShader)
        return nullptr;
    
    // Create shader program and attach vertex and fragment shader to it
    s->shaderProgram = glCreateProgram();
    s->attachShader(vertShader.value());
    s->attachShader(fragShader.value());
    
    s->link();
    // Vertex and fragment are already attached to program, so we can delete them.
    glDeleteShader(vertShader.value());
    glDeleteShader(fragShader.value());
    
    return s;
}

std::optional<GLuint> Shader::compileShader(const char *source, GLenum type) {
    GLuint shader = glCreateShader(type);
    
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
    GLint result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if(result == GL_FALSE) {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> fragShaderError( (logLength > 1) ? logLength : 1 );
        glGetShaderInfoLog(shader, logLength, NULL, &fragShaderError[0]);
        fprintf(stderr, "%s\n", &fragShaderError[0]);
        glDeleteShader(shader);
        return {};
    }
    return shader;
}

std::optional<std::pair<std::string, std::string>> Shader::readFile(const char *filepath) {
    std::ifstream in(filepath);
    
    if(!in) return {};
    
    std::string line;
    std::string vertexSource;
    std::string fragmentSource;
    int currentShader = 0;
    while(std::getline(in, line)) {
        if(line == "#type vertex") {
            currentShader = GL_VERTEX_SHADER;
            continue;
        } else if(line == "#type fragment") {
            currentShader = GL_FRAGMENT_SHADER;
            continue;
        }
        if(currentShader == GL_VERTEX_SHADER)
            vertexSource += line + "\n";
        else if(currentShader == GL_FRAGMENT_SHADER)
            fragmentSource += line + "\n";
        else {
            fprintf(stderr, "Bad shader formatting\n");
            continue;
        }
    }
    in.close();
    return std::pair{ vertexSource, fragmentSource };
}

void Shader::attachShader(GLint shader) const {
    glAttachShader(shaderProgram, shader);
}

GLint Shader::getUniformLocation(const char *name) const {
    GLint location = glGetUniformLocation(shaderProgram, name);
    if(location == -1)
        fprintf(stderr, "Non fatal: Uniform %s does not exist\n", name);
    return location;
}

void Shader::freeShader(Shader *s) {
    if(s != nullptr) {
        glDeleteShader(s->shaderProgram);
        delete s;
    }
}

void Shader::use() const {
    glUseProgram(shaderProgram);
}

void Shader::link() const {
    glLinkProgram(shaderProgram);
}

void Shader::enableVertexAttribArray(const char *name, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) const {
    GLint attrib = glGetAttribLocation(shaderProgram, name);
    glVertexAttribPointer(attrib, size, type, normalized, stride, pointer);
    glEnableVertexAttribArray(attrib);
}

GLint Shader::getAttribLocation(const char *name) const {
    return glGetAttribLocation(shaderProgram, name);
}

void Shader::bindFragDataLocation(const GLuint colorNumber, const char *name) const {
    glBindFragDataLocation(shaderProgram, colorNumber, name);
}

void Shader::setUniform1i(const int location, const GLint value) const {
    glUniform1i(location, value);
}

void Shader::setUniform1i(const char *name, const GLint value) const {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setUniform1f(const char *name, const GLfloat value) const {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setUniform1f(const int location, const GLfloat value) const {
    glUniform1f(location, value);
}

void Shader::setUniformMat4f(const char *name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setUniformMat4f(const int location, const glm::mat4 &mat) const {
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}
