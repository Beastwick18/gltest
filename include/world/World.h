#ifndef MINECRAFT_CLONE_WORLD_H
#define MINECRAFT_CLONE_WORLD_H

#include "world/Chunk.h"
#include <mutex>

struct RaycastResult {
    bool hit;
    glm::ivec3 hitCoords;
    glm::ivec3 hitSide;
    BlockID block;
    // possibly add "liquidHit", which indicates a hit liquid. or maybe just have
    // another result that contains the liquid details
};

namespace World {
    extern std::vector<Chunk> chunks;
    extern std::mutex chunkMutex;
    
    RaycastResult raycast(const glm::vec3 startPos, const glm::vec3 dir, const float length);
    
    Chunk *getChunk(const glm::ivec2 pos);
    Chunk *getChunk(const int x, const int z);
   
    BlockID getBlock(const Chunk *chunk, const glm::ivec3 pos);
    BlockID getBlock(const Chunk *chunk, const int x, const int y, const int z);
    BlockID getBlock(const glm::ivec3 pos);
    BlockID getBlock(const int x, const int y, const int z);
    
    void addBlock(Chunk *chunk, const BlockID id, const glm::ivec3 pos);
    void addBlock(Chunk *chunk, const BlockID id, const int x, const int y, const int z);
    void addBlock(const BlockID id, const glm::ivec3 pos);
    void addBlock(const BlockID id, const int x, const int y, const int z);
    
    void removeBlock(Chunk *chunk, const glm::ivec3 pos);
    void removeBlock(Chunk *chunk, const int x, const int y, const int z);
    void removeBlock(const glm::ivec3 pos);
    void removeBlock(const int x, const int y, const int z);
    
    AdjChunks getAdjacentChunks(const glm::ivec2 chunkPos);
}

#endif
