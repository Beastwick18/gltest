#include <optional>
#include <string>

class ComputeShader {
    static ComputeShader *createShader(const char *shaderPath, glm::uvec3 workSize);
    static void free(ComputeShader *c);
    
    static std::optional<std::string> readFile(const char *filepath);
    
    void use() const;
    void dispatch() const;
};
