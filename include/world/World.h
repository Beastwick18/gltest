#ifndef MINECRAFT_CLONE_WORLD_H
#define MINECRAFT_CLONE_WORLD_H

#include <mutex>
#include <map>
#include "world/Chunk.h"

struct RaycastResult {
    bool hit;
    glm::ivec3 hitCoords;
    glm::ivec3 hitSide;
    BlockID blockID;
};

struct RaycastResults {
    RaycastResult block;
    RaycastResult liquid;
};

struct SurroundingBlocks {
    BlockID left, right, top, bottom, front, back;
    SurroundingBlocks operator!() {
        return {!left, !right, !top, !bottom, !front, !back};
    }
};

namespace World {
    extern std::map<std::string, Chunk> chunks;
    extern std::mutex chunkMutex;
    
    RaycastResults raycast(const glm::vec3 startPos, const glm::vec3 dir, const float length);
    
    
    glm::ivec2 toChunkCoords(glm::vec3 pos);
    glm::ivec2 toChunkCoords(int x, int z);
    
    Chunk *getChunk(const glm::ivec2 pos);
    Chunk *getChunk(const int x, const int z);
   
    BlockID getBlock(const Chunk *chunk, const glm::ivec3 pos);
    BlockID getBlock(const Chunk *chunk, const int x, const int y, const int z);
    BlockID getBlock(const glm::ivec3 pos);
    BlockID getBlock(const int x, const int y, const int z);
    SurroundingBlocks getAdjacentBlocks(const glm::ivec3 pos);
    SurroundingBlocks getAdjacentBlocks(const int x, const int y, const int z);
    
    void addBlock(Chunk *chunk, const BlockID id, const glm::ivec3 pos);
    void addBlock(Chunk *chunk, const BlockID id, const int x, const int y, const int z);
    void addBlock(const BlockID id, const glm::ivec3 pos);
    void addBlock(const BlockID id, const int x, const int y, const int z);
    
    void removeBlock(Chunk *chunk, const glm::ivec3 pos);
    void removeBlock(Chunk *chunk, const int x, const int y, const int z);
    void removeBlock(const glm::ivec3 pos);
    void removeBlock(const int x, const int y, const int z);
    
    std::string generateChunkKey(const glm::ivec2 pos);
    AdjChunks getAdjacentChunks(const glm::ivec2 chunkPos);
}

#endif
