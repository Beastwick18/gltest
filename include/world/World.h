#ifndef MINECRAFT_CLONE_WORLD_H
#define MINECRAFT_CLONE_WORLD_H

#include "world/Chunk.h"
#include <mutex>

namespace World {
    extern std::vector<Chunk> chunks;
    extern std::mutex chunkMutex;
    BlockID getBlock(int x, int y, int z);
    void addBlock(BlockID id, int x, int y, int z);
    void removeBlock(int x, int y, int z);
    AdjChunks getAdjacentChunks(glm::ivec2 chunkPos);
}

#endif
