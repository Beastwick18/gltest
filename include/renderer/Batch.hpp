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
    // glm::vec3 normal;
    glm::vec2 texCoords;
    unsigned int data;
    // float light, skyLight;
};

struct Vertex2D {
    glm::vec2 position;
    glm::vec2 texCoords;
};

enum MeshType {
    CUBE,
    LIQUID,
    TORCH,
    CROSS
};
template<typename T>
struct Mesh {
    Vertex *data;
    size_t numVertices;
    size_t maxVertices;
    
    Mesh() {
        numVertices = 0;
        maxVertices = 0;
        data = nullptr;
    }
    
    void init(size_t maxVertices) {
        this->maxVertices = maxVertices;
        data = (T*)calloc(maxVertices, sizeof(T));
    }
    
    void free() {
        numVertices = 0;
        delete data;
    }
    
    void clear() {
        numVertices = 0;
    }
    
    bool addVertex(const T &t) {
        if(numVertices >= maxVertices) return false;
        data[numVertices++] = t;
        return true;
    }
    
    bool addVertices(const T *arr, const size_t size) {
        if(numVertices + size >= maxVertices) return false;
        std::copy(arr, arr+size, data+numVertices);
        numVertices += size;
        return true;
    }
    
    size_t size() const {
        return numVertices;
    }
    
    size_t rawSize() const {
        return numVertices * sizeof(T);
    }
    
    bool empty() const {
        return numVertices == 0;
    }
    
    bool full() const {
        return numVertices >= maxVertices-1;
    }
    
    inline bool hasRoomFor(const size_t vertices) const {
        return numVertices+vertices < maxVertices;
    }
};

template<typename T>
class Batch {
    public:
        Batch() {}
        
        ~Batch() {}
        
        void init(const VBlayout &layout) {
            maxVertices = (verticesInKilobytes * 1024)/sizeof(T);
            // vertices = (T*)calloc(maxVertices, sizeof(T));
            // mesh.init(maxVertices);
            
            vao = new VAO;
            vbo = new VBO(sizeof(T) * maxVertices);
            vao->addBuffer(vbo, layout);
            
            // vertexCount = 0;
            // sizeOfVertex = sizeof(T);
        }
        
        bool addVertex(const T &t) {
            // if(!isFull() || vertices == nullptr) return false;
            // vertices[vertexCount++] = t;
            // return true;
            return mesh.addVertex(t);
        }
        
        bool addVertices(const T *arr, const GLsizeiptr size) {
            // if(!hasRoomFor(size) || vertices == nullptr) return false;
            // std::copy(arr, arr+size, vertices+vertexCount);
            // vertexCount += size;
            // return true;
            return mesh.addVertices(arr, size);
        }
        
        void flush() {
            if(mesh.empty()) return;
            
            vbo->setData(mesh.data, mesh.rawSize());
            vao->bind();
            glDrawArrays(GL_TRIANGLES, 0, mesh.size());
            DebugStats::drawCalls++;
            mesh.clear();
        }
        
        void flushMesh(const Mesh<T> &m) {
            if(m.empty() || m.size() > maxVertices) return;
            
            vbo->setData(m.data, m.rawSize());
            vao->bind();
            glDrawArrays(GL_TRIANGLES, 0, m.size());
            DebugStats::drawCalls++;
        }
        
        void free() {
            // if(vertices != nullptr) {
                // std::free(vertices);
                // vertices = nullptr;
                // vertexCount = 0;
                VAO::free(vao);
                VBO::free(vbo);
            // }
                mesh.free();
        }
        inline bool isFull() const {
            return mesh.fUll();
        }
        
        inline bool hasRoomFor(const size_t vertices) const {
            return mesh.hasRoomFor(vertices);
        }
        
        inline bool isEmpty() const {
            return mesh.empty();
        }
        
    private:
        // unsigned int vertexCount = 0;
        // static constexpr size_t verticesInKilobytes = 1024;
        // static constexpr size_t verticesInKilobytes = 511872;
        static constexpr size_t verticesInKilobytes = 4096;
        Mesh<T> mesh;
        size_t maxVertices;
        // size_t sizeOfVertex;
        // T* vertices = nullptr;
        VBO *vbo = nullptr;
        VAO *vao = nullptr;
};

#endif
