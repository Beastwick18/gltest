#ifndef MINECRAFT_CLONE_BATCH_H
#define MINECRAFT_CLONE_BATCH_H

// #include <array>
#include "glm/glm.hpp"
#include "core.h"
#include "renderer/VAO.h"
#include "input/input.h"

struct Vertex {
    glm::vec3 position;
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
            // Give us 4 MB of space
            maxVertices = (verticesInKilobytes * 1024)/sizeof(T);
            verticesSize = sizeof(T) * maxVertices;
            vertices = (T*)calloc(maxVertices, sizeof(T));
            
            vao = new VAO;
            vao->bind();
            vbo = new VBO(verticesSize);
            vao->addBuffer(vbo, layout);
            
            numVertices = 0;
        }
        
        void addVertex(const T t) {
            if(vertices == nullptr || numVertices >= maxVertices-1) {
                fprintf(stderr, "Non fatal: No more space in batch to add vertex\n");
                return;
            }
            if(numVertices < 0) {
                fprintf(stderr, "Non fatal: Invalid vertex number\n");
                return;
            }
            // printf("maxVertices: %d, numVertices: %d, vsize: %d\n", maxVertices, numVertices, verticesSize);
            vertices[numVertices] = t;
            numVertices++;
        }
        
        void flush() {
            vbo->bind();
            // Only send the part of the buffer that has been used
            glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(T), vertices, GL_DYNAMIC_DRAW);
            // glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_DYNAMIC_DRAW);
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
        
        bool hasRoomFor(int vertices) {
            return numVertices+vertices < maxVertices;
        }
        
        unsigned int numVertices = 0;
    private:
        // 4 MB of vertices
        static constexpr size_t verticesInKilobytes = 4096;
        size_t maxVertices;
        T* vertices = nullptr;
        GLsizeiptr verticesSize = 0;
        VBO *vbo = nullptr;
        VAO *vao = nullptr;
};

#endif
