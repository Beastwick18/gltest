#ifndef MINECRAFT_CLONE_BATCH_H
#define MINECRAFT_CLONE_BATCH_H

#include "glm/glm.hpp"
#include "core.h"
#include "renderer/VBlayout.h"
#include "renderer/VAO.h"
#include "renderer/VBO.h"
#include "renderer/EBO.h"
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
    T *data;
    GLuint *indices;
    size_t numVertices;
    size_t maxVertices;
    size_t numIndices;
    size_t maxIndices;
    
    Mesh() {
        numVertices = 0;
        maxVertices = 0;
        numIndices = 0;
        maxIndices = 0;
        data = nullptr;
    }
    
    void init(size_t maxVertices, size_t maxIndices) {
        this->maxVertices = maxVertices;
        this->maxIndices = maxIndices;
        data = (T*)calloc(maxVertices, sizeof(T));
        indices = (GLuint *) calloc(maxIndices, sizeof(GLuint));
    }
    
    void free() {
        numVertices = 0;
        numIndices = 0;
        delete data;
        delete indices;
    }
    
    void clear() {
        numVertices = 0;
        numIndices = 0;
    }
    
    // bool addVertex(const T &t) {
    //     if(numVertices >= maxVertices) return false;
    //     data[numVertices++] = t;
    //     return true;
    // }
    
    bool addVertices(const T *arr, const size_t size, const GLuint *idc, const size_t idc_size) {
        if(numVertices + size >= maxVertices) return false;
        std::copy(arr, arr+size, data+numVertices);
        std::copy(idc, idc+idc_size, indices+numIndices);
        for(int i = numIndices; i < numIndices+idc_size; i++) {
            indices[i] += numVertices;
        }
        numVertices += size;
        numIndices += idc_size;
        return true;
    }
    
    size_t size() const {
        return numIndices;
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
            ebo = new EBO(maxIndices);
            
            // vertexCount = 0;
            // sizeOfVertex = sizeof(T);
        }
        
        bool addVertex(const T &t) {
            // if(!isFull() || vertices == nullptr) return false;
            // vertices[vertexCount++] = t;
            // return true;
            return mesh.addVertex(t);
        }
        
        bool addVertices(const T *arr, const GLsizeiptr size, const GLuint *idc, const GLsizeiptr idc_size) {
            // if(!hasRoomFor(size) || vertices == nullptr) return false;
            // std::copy(arr, arr+size, vertices+vertexCount);
            // vertexCount += size;
            // return true;
            return mesh.addVertices(arr, size, idc, idc_size);
        }
        
        void flush() {
            if(mesh.empty()) return;
            
            vbo->setData(mesh.data, mesh.rawSize());
            vao->bind();
            ebo->bind();
            // glDrawArrays(GL_TRIANGLES, 0, mesh.size());
            glDrawElements(GL_TRIANGLES, ebo->getCount(), GL_UNSIGNED_INT, nullptr);
            DebugStats::drawCalls++;
            mesh.clear();
        }
        
        void flushMesh(const Mesh<T> &m) {
            if(m.empty() || m.size() > maxVertices) return;
            
            vbo->setData(m.data, m.rawSize());
            ebo->setData(m.indices, m.size() * sizeof(GLuint));
            vao->bind();
            ebo->bind();
            glDrawElements(GL_TRIANGLES, m.size(), GL_UNSIGNED_INT, nullptr);
            // glDrawArrays(GL_TRIANGLES, 0, m.size());
            DebugStats::drawCalls++;
        }
        
        void free() {
            VAO::free(vao);
            VBO::free(vbo);
            EBO::free(ebo);
            
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
        size_t maxIndices;
        // size_t sizeOfVertex;
        // T* vertices = nullptr;
        VBO *vbo = nullptr;
        VAO *vao = nullptr;
        EBO *ebo = nullptr;
};

#endif
