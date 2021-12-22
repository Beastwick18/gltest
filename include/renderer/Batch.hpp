#ifndef MINECRAFT_CLONE_BATCH_H
#define MINECRAFT_CLONE_BATCH_H

// #include <array>
#include "glm/glm.hpp"
#include "core.h"
#include "renderer/VAO.h"
#include "input/input.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    // float textureIndex;
};

struct Vertex2D {
    glm::vec2 position;
    glm::vec2 texCoords;
    // float textureIndex;
};

// use offsetof macro
template<typename T>
class Batch {
    public:
        Batch() {}
        
        ~Batch() {}
        
        void init(const VBlayout &layout) {
            maxVertices = (verticesInKilobytes * 1024)/sizeof(T);
            vertices = (T*)calloc(maxVertices, sizeof(T));
            
            vao = new VAO;
            vao->bind();
            vbo = new VBO(sizeof(T) * maxVertices);
            vao->addBuffer(vbo, layout);
            vbo->bind();
            
            numVertices = 0;
        }
        
        bool addVertex(const T &t) {
            if(hasRoomFor(1) || vertices == nullptr) return false;
            vertices[numVertices++] = t;
            return true;
        }
        
        bool addVertices(const T *arr, const GLsizeiptr size) {
            if(!hasRoomFor(size) || vertices == nullptr) return false;
            std::copy(arr, arr+size, vertices+numVertices);
            numVertices += size;
            return true;
        }
        
        void flush() {
            vbo->bind();
            // Only send the part of the buffer that has been used
            glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(T), vertices, GL_DYNAMIC_DRAW);
            vao->bind();
            glDrawArrays(GL_TRIANGLES, 0, numVertices);
            numVertices = 0;
        }
        
        void free() {
            if(vertices != nullptr) {
                std::free(vertices);
                vertices = nullptr;
                numVertices = 0;
                VAO::free(vao);
                VBO::free(vbo);
            }
        }
        bool isFull() const {
            return numVertices == maxVertices-1;
        }
        
        bool hasRoomFor(const int vertices) const {
            return numVertices+vertices < maxVertices;
        }
        
        unsigned int numVertices = 0;
    private:
        // 2 MB of vertices
        // static constexpr size_t verticesInKilobytes = 4096-1024*3;
        static constexpr size_t verticesInKilobytes = 1024;
        // static constexpr size_t verticesInKilobytes = 512;
        size_t maxVertices;
        T* vertices = nullptr;
        VBO *vbo = nullptr;
        VAO *vao = nullptr;
};

#endif
