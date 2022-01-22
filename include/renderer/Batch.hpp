#ifndef MINECRAFT_CLONE_BATCH_H
#define MINECRAFT_CLONE_BATCH_H

#include "glm/glm.hpp"
#include "core.h"
#include "renderer/VBlayout.h"
#include "renderer/VAO.h"
#include "renderer/VBO.h"
#include "utils/DebugStats.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Vertex2D {
    glm::vec2 position;
    glm::vec2 texCoords;
};

template<typename T>
class Batch {
    public:
        Batch() {}
        
        ~Batch() {}
        
        void init(const VBlayout &layout) {
            maxVertices = (verticesInKilobytes * 1024)/sizeof(T);
            vertices = (T*)calloc(maxVertices, sizeof(T));
            
            vao = new VAO;
            vbo = new VBO(sizeof(T) * maxVertices);
            vao->addBuffer(vbo, layout);
            
            vertexCount = 0;
            sizeOfVertex = sizeof(T);
        }
        
        bool addVertex(const T &t) {
            if(!isFull() || vertices == nullptr) return false;
            vertices[vertexCount++] = t;
            return true;
        }
        
        bool addVertices(const T *arr, const GLsizeiptr size) {
            if(!hasRoomFor(size) || vertices == nullptr) return false;
            std::copy(arr, arr+size, vertices+vertexCount);
            vertexCount += size;
            return true;
        }
        
        void flush() {
            if(vertexCount == 0) return;
            
            vbo->setData(vertices, vertexCount * sizeOfVertex);
            vao->bind();
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);
            DebugStats::drawCalls++;
            vertexCount = 0;
        }
        
        void free() {
            if(vertices != nullptr) {
                std::free(vertices);
                vertices = nullptr;
                vertexCount = 0;
                VAO::free(vao);
                VBO::free(vbo);
            }
        }
        inline bool isFull() const {
            return vertexCount >= maxVertices-1;
        }
        
        inline bool hasRoomFor(const int vertices) const {
            return vertexCount+vertices < maxVertices;
        }
        
        inline bool isEmpty() const {
            return vertexCount == 0;
        }
        
    private:
        unsigned int vertexCount = 0;
        static constexpr size_t verticesInKilobytes = 1024;
        // static constexpr size_t verticesInKilobytes = 511872;
        // static constexpr size_t verticesInKilobytes = 4096;
        size_t maxVertices;
        size_t sizeOfVertex;
        T* vertices = nullptr;
        VBO *vbo = nullptr;
        VAO *vao = nullptr;
};

#endif
