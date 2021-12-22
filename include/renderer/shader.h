#ifndef MINCRAFT_CLONE_SHADER_H
#define MINCRAFT_CLONE_SHADER_H

#include "core.h"
#include "glm/glm.hpp"
#include <optional>

class Shader {
    private:
        GLuint shaderProgram;
        
        static std::optional<GLuint> compileShader(const char *source, GLenum type);
        
    public:
        static Shader *createShader(const char *shaderPath);
        static void freeShader(Shader *s);
        
        static std::optional<std::pair<std::string, std::string>> readFile(const char *filepath);
        
        void use() const;
        void link() const;
        
        void attachShader(GLint shader) const;
        
        void enableVertexAttribArray(const char *name, GLint size, GLenum type, GLboolean normalized, GLsizei stride = 0, const void *pointer = nullptr) const;
        
        void bindFragDataLocation(const GLuint colorNumber, const char *name) const;
        
        GLint getUniformLocation(const char *name) const;
        
        GLint getAttribLocation(const char *name) const;
        
        void setUniform1i(const char *name, const GLint v1) const;
        void setUniform1i(const int location, const GLint v1) const;
        // void setUniform2i(const char *name, GLint v1, GLint v2);
        // void setUniform3i(const char *name, GLint v1, GLint v2, GLint v3);
        // void setUniform4i(const char *name, GLint v1, GLint v2, GLint v3, GLint v4);
        
        // void setUniform1iv(const char *name, const GLint *value);
        // void setUniform2iv(const char *name, const GLint *value);
        // void setUniform3iv(const char *name, const GLint *value);
        // void setUniform4iv(const char *name, const GLint *value);
        
        // void setUniform1ui(const char *name, GLuint v1);
        // void setUniform2ui(const char *name, GLuint v1, GLuint v2);
        // void setUniform3ui(const char *name, GLuint v1, GLuint v2, GLuint v3);
        // void setUniform4ui(const char *name, GLuint v1, GLuint v2, GLuint v3, GLuint v4);
        
        // void setUniform1uiv(const char *name, const GLfloat *value);
        // void setUniform2uiv(const char *name, const GLfloat *value);
        // void setUniform3uiv(const char *name, const GLfloat *value);
        // void setUniform4uiv(const char *name, const GLfloat *value);
        
        void setUniform1f(const char *name, GLfloat v1) const;
        void setUniform1f(const int location, GLfloat v1) const;
        // void setUniform2f(const char *name, GLfloat v1, GLfloat v2);
        // void setUniform3f(const char *name, GLfloat v1, GLfloat v2, GLfloat v3);
        // void setUniform4f(const char *name, GLfloat v1, GLfloat v2, GLfloat v3, GLfloat v4);
        
        // void setUniform1fv(const char *name, GLfloat *value);
        // void setUniform2fv(const char *name, GLfloat *values);
        // void setUniform3fv(const char *name, GLfloat *values);
        // void setUniform4fv(const char *name, GLfloat *values);
        
        // void setUniformMatrix2fv(const char *name, const GLfloat *value);
        // void setUniformMatrix3fv(const char *name, const GLfloat *value);
        void setUniformMat4f(const char *name, const glm::mat4 &mat) const;
        void setUniformMat4f(const int location, const glm::mat4 &mat) const;
        
        // void setUniformMatrix2x3fv(const char *name, const GLfloat *value);
        // void setUniformMatrix3x2fv(const char *name, const GLfloat *value);
        // void setUniformMatrix2x4fv(const char *name, const GLfloat *value);
        // void setUniformMatrix4x2fv(const char *name, const GLfloat *value);
        // void setUniformMatrix3x4fv(const char *name, const GLfloat *value);
        // void setUniformMatrix4x3fv(const char *name, const GLfloat *value);
};

#endif
