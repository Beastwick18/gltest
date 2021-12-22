#ifndef MINECRAFT_CLONE_CHUNK_H
#define MINECRAFT_CLONE_CHUNK_H

#include "renderer/Batch.hpp"

enum class ChunkStatus {
    EMPTY,
    BUILT,
    BUILDING
};

typedef unsigned char BlockID;

struct ChunkMesh {
    std::vector<Vertex> v;
};

class Chunk {
    Chunk(int x, int y);
    ~Chunk();
    void generateChunk();
    void rebuildMesh();
    static float getNoise(float x, float z);
    inline ChunkMesh getMesh() { return mesh; }
    inline glm::vec2 getPos() { return pos; }
    inline ChunkStatus getStatus() { return status; }
private:
    static const int chunkW = 16, chunkL = 16, chunkH = 256;
    glm::ivec2 pos;
    ChunkMesh mesh;
    ChunkStatus status;
    BlockID blocks[chunkH][chunkW][chunkL];
};

#endif
