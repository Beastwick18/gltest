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

class Chunk {
public:
    Chunk(int x, int z);
    Chunk();
    ~Chunk();
    
    void generateChunk();
    void calculateSkyLighting(int x = 0, int y = chunkH-1, int z = 0, float prev = 1.f);
    void calculateSkyLightingSpread(int x = 0, int y = chunkH-1, int z = 0, float prev = 1.f);
    void calculateLighting(int x = 0, int y = 0, int z = 0, float prev = 1.f);
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
    static const int chunkW = 32, chunkL = 32, chunkH = 150;
    // static const int chunkW = 8, chunkL = 8, chunkH = 150;
    // static const int chunkW = 16, chunkL = 16, chunkH = 256;
private:
    glm::ivec2 pos;
    Mesh<Vertex> mesh;
    Mesh<Vertex> transparentMesh;
    ChunkStatus status;
    BlockID blocks[chunkH][chunkW][chunkL];
    unsigned char light[chunkH][chunkW][chunkL];
    unsigned char skyLightSpread[chunkH][chunkW][chunkL];
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
