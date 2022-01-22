#ifndef MINECRAFT_CLONE_CHUNK_H
#define MINECRAFT_CLONE_CHUNK_H

#include "renderer/Batch.hpp"
#include "renderer/Frustum.h"
#include "world/Block.h"
#include "renderer/renderer.h"

enum ChunkStatus {
    EMPTY,
    BUILDING,
    BUILT,
    HIDDEN,
    MESHING,
    SHOWING
};

struct ChunkMesh {
    std::vector<Vertex> v;
};

class Chunk {
public:
    Chunk(int x, int z);
    Chunk();
    ~Chunk();
    
    void generateChunk();
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
    inline const ChunkMesh& getMesh() const { return mesh; }
    inline const ChunkMesh& getTransparentMesh() const { return transparentMesh; }
    inline glm::ivec2 getPos() const { return pos; }
    inline ChunkStatus getStatus() const { return status; }
    static const int chunkW = 32, chunkL = 32, chunkH = 150;
    // static const int chunkW = 8, chunkL = 8, chunkH = 150;
    // static const int chunkW = 16, chunkL = 16, chunkH = 256;
private:
    glm::ivec2 pos;
    ChunkMesh mesh;
    ChunkMesh transparentMesh;
    ChunkStatus status;
    BlockID blocks[chunkH][chunkW][chunkL];
    int maxY, minY;
};

struct AdjChunks {
    Chunk *left;
    Chunk *right;
    Chunk *front;
    Chunk *back;
};

#endif
