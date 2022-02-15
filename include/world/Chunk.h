#ifndef MINECRAFT_CLONE_CHUNK_H
#define MINECRAFT_CLONE_CHUNK_H

#include "renderer/Batch.hpp"
#include "renderer/Frustum.h"
#include "world/Block.h"

enum ChunkStatus {
    EMPTY,
    BUILDING,
    BUILT,
    HIDDEN,
    MESHING,
    SHOWING
};


struct Light {
    glm::ivec3 pos;
    float val;
};

struct AdjChunks;
struct SurroundingBlocks;

class Chunk {
public:
    Chunk(int x, int z);
    Chunk();
    ~Chunk();
    
    void generateChunk();
    void calculateSkyLighting(int x = 0, int y = chunkH-1, int z = 0, float prev = 1.f);
    void calculateSkyLightingSpread(int x = 0, int y = chunkH-1, int z = 0, float prev = 1.f);
    void calculateLighting(int x = 0, int y = 0, int z = 0, float prev = 1.f);
    void generateQuadMesh(Mesh<Vertex> &mesh, Vertex v0, Vertex v1, Vertex v2, Vertex v3);
    void generateCubeMesh(Mesh<Vertex> &mesh, int x, int y, int z, BlockTexture tex, SurroundingBlocks adj);
    void generateLiquidMesh(Mesh<Vertex> &mesh, int x, int y, int z, BlockTexture tex, SurroundingBlocks adj);
    void generateTorchMesh(Mesh<Vertex> &mesh, int x, int y, int z, BlockTexture tex, SurroundingBlocks adj);
    void rebuildMesh();
    BlockID getBlock(int x, int y, int z) const;
    void addBlock(BlockID id, int x, int y, int z);
    void removeBlock(int x, int y, int z);
    void show();
    void hide();
    bool isVisible(const Frustum *f) const;
    bool operator<(const Chunk& other) const;
    static float getNoise(glm::vec2 position);
    static float getNoise(float x, float z);
    inline const Mesh<Vertex>& getMesh() const { return mesh; }
    inline const Mesh<Vertex>& getTransparentMesh() const { return transparentMesh; }
    inline glm::ivec2 getPos() const { return pos; }
    inline ChunkStatus getStatus() const { return status; }
    float getLight(int x, int y, int z);
    float getSkyLight(int x, int y, int z);
    static const int chunkW = 32, chunkL = 32, chunkH = 150;
    // static const int chunkW = 8, chunkL = 8, chunkH = 150;
    // static const int chunkW = 16, chunkL = 16, chunkH = 256;
private:
    glm::ivec2 pos;
    Mesh<Vertex> mesh;
    Mesh<Vertex> transparentMesh;
    ChunkStatus status;
    BlockID blocks[chunkH][chunkW][chunkL];
    
    // Can fit 0-15 in 4 bits, consider packing two values into one byte
    // Shift byte, use 15 (0b1111) as a mask
    // Do this for both light and skyLight
    unsigned char light[chunkH][chunkW][chunkL];
    unsigned char skyLight[chunkH][chunkW][chunkL];
    int maxY, minY;
};

struct AdjChunks {
    Chunk *left;
    Chunk *right;
    Chunk *front;
    Chunk *back;
};

#endif
