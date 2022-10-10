#include "world/World.h"
#include <future>
#include "glm/gtc/constants.hpp"

namespace World {
    std::map<std::string, Chunk> chunks;
    std::vector<std::future<void>> meshFutures;
    std::mutex chunkMutex;
    
    std::string generateChunkKey(const glm::ivec2 pos) {
        return std::to_string(pos.x) + "," + std::to_string(pos.y);
    }
    
    glm::ivec2 toChunkCoords(glm::vec3 pos) {
        return toChunkCoords(pos.x, pos.z);
    }
    
    glm::ivec2 toChunkCoords(int x, int z) {
        return {Chunk::chunkW * (x / Chunk::chunkW), Chunk::chunkL * (z / Chunk::chunkL)};
    }
    
    // Ray casting :D
    RaycastResults raycast(const glm::vec3 startPos, const glm::vec3 dir, const float length) {
        RaycastResults result;
        result.block.hit = false;
        result.liquid.blockID = Blocks::NULL_BLOCK;
        result.block.blockID = Blocks::NULL_BLOCK;
        
        glm::vec3 tMax, tDelta, pos, step;
        
        pos = glm::floor(startPos);
        step = glm::sign(dir);
        Chunk *currChunk = getChunk(pos.x, pos.z);
        BlockID blockID = getBlock(currChunk, pos);
        tMax = (pos + (step * .5f) + .5f - startPos) / dir;
        tDelta = step / dir;
        
        int side = 0;
        float min;
        
        if((result.liquid.hit = Blocks::getBlockFromID(blockID).liquid)) {
            result.liquid = {.hitCoords = pos, .hitSide = {0, 0, 0}, .blockID = blockID};
            blockID = Blocks::AIR_BLOCK;
        }
        
        while(currChunk && glm::distance(startPos, pos) <= length && blockID == Blocks::AIR_BLOCK && blockID != Blocks::NULL_BLOCK) {
            min = glm::min(tMax.x, glm::min(tMax.y, tMax.z));
            for(side = 0; side < tMax.length(); side++)
                if(tMax[side] == min) {
                    pos[side] += step[side];
                    tMax[side] += tDelta[side];
                    break;
                }
            
            if(auto cpos = toChunkCoords(pos); cpos != currChunk->getPos())
                currChunk = getChunk(cpos);
            blockID = getBlock(currChunk, pos.x, pos.y, pos.z);
            if(Blocks::getBlockFromID(blockID).liquid) {
                if(!result.liquid.hit) {
                    result.liquid = {true, pos, glm::ivec3(0), blockID};
                    result.liquid.hitSide[side] = -step[side];
                }
                blockID = Blocks::AIR_BLOCK;
            }
        }
        
        if(blockID != Blocks::NULL_BLOCK && blockID != Blocks::AIR_BLOCK && !Blocks::getBlockFromID(blockID).liquid) {
            result.block = { true, pos, glm::ivec3(0), blockID };
            result.block.hitSide[side] = -step[side];
        }
        return result; 
    }
    
    Chunk *getChunk(const glm::ivec2 pos) {
        return getChunk(pos.x, pos.y);
    }
    
    Chunk *getChunk(const int x, const int z) {
        glm::ivec2 chunkPos = toChunkCoords(x, z);
        
        auto i = chunks.find(generateChunkKey(chunkPos));
        if(i == chunks.end()) return nullptr;
        return &i->second;
    }
    
    BlockID getBlock(const Chunk *chunk, const glm::ivec3 pos) {
        return getBlock(chunk, pos.x, pos.y, pos.z);
    }
    
    BlockID getBlock(const Chunk *chunk, const int x, const int y, const int z) {
        if(chunk == nullptr) return Blocks::NULL_BLOCK;
        
        const auto &pos = chunk->getPos();
        return chunk->getBlock(x - pos.x, y, z - pos.y);
    }
    
    BlockID getBlock(const glm::ivec3 pos) {
        return getBlock(pos.x, pos.y, pos.z);
    }
    
    BlockID getBlock(const int x, const int y, const int z) {
        auto chunk = getChunk(x, z);
        if(chunk == nullptr) return Blocks::NULL_BLOCK;
        return getBlock(chunk, x, y, z);
    }
    
    SurroundingBlocks getAdjacentBlocks(const glm::ivec3 pos) {
        return getAdjacentBlocks(pos.x, pos.y, pos.z);
    }
    
    SurroundingBlocks getAdjacentBlocks(const int x, const int y, const int z) {
        SurroundingBlocks s;
        const Chunk *c = getChunk(x, z);
        const auto &cpos = c->getPos();
        s.top = World::getBlock(c, x, y+1, z);
        s.bottom = World::getBlock(c, x, y-1, z);
        
        if(const auto &pos = toChunkCoords(x-1, z); pos == cpos)
            s.left = World::getBlock(c, x-1, y, z);
        else
            s.left = World::getBlock(getChunk(pos), x-1, y, z);
        
        if(const auto &pos = toChunkCoords(x+1, z); pos == cpos)
            s.right = World::getBlock(c, x+1, y, z);
        else
            s.right = World::getBlock(getChunk(pos), x+1, y, z);
        
        if(const auto &pos = toChunkCoords(x, z+1); pos == cpos)
            s.front = World::getBlock(c, x, y, z+1);
        else
            s.front = World::getBlock(getChunk(pos), x, y, z+1);
        
        if(const auto &pos = toChunkCoords(x, z-1); pos == cpos)
            s.back = World::getBlock(c, x, y, z-1);
        else
            s.back = World::getBlock(getChunk(pos), x, y, z-1);
        
        return s;
    }
    
    void addBlock(Chunk *chunk, const BlockID id, const glm::ivec3 pos) {
        addBlock(chunk, id, pos.x, pos.y, pos.z);
    }
    
    void addBlock(Chunk *chunk, const BlockID id, const int x, const int y, const int z) {
        if(chunk == nullptr) return;
        
        auto pos = chunk->getPos();
        int rx = x - pos.x;
        int rz = z - pos.y;
        chunk->addBlock(id, rx, y, rz);
        // meshFutures.push_back(std::async(std::launch::async, &Chunk::rebuildMesh, chunk));
        chunk->fullRebuildMesh();
        bool left = rx == 0, right = rx == Chunk::chunkW-1, back = rz == 0, front = rz == Chunk::chunkL-1;
        if(left || right || front || back) {
            AdjChunks adj = getAdjacentChunks(pos);
            if(left && adj.left)
                adj.left->fullRebuildMesh();
            if(right && adj.right)
                adj.right->fullRebuildMesh();
            if(front && adj.front)
                adj.front->fullRebuildMesh();
            if(back && adj.back)
                adj.back->fullRebuildMesh();
        }
        
    }
    
    void addBlock(const BlockID id, const glm::ivec3 pos) {
        addBlock(id, pos.x, pos.y, pos.z);
    }
    
    void addBlock(BlockID id, const int x, const int y, const int z) {
        Chunk *chunk = getChunk(x, z);
        if(chunk == nullptr) return;
        addBlock(chunk, id, x, y, z);
    }
    
    void removeBlock(Chunk *chunk, const glm::ivec3 pos) {
        removeBlock(chunk, pos.x, pos.y, pos.z);
    }
    
    void removeBlock(Chunk *chunk, const int x, const int y, const int z) {
        if(chunk == nullptr) return;
        
        const auto &pos = chunk->getPos();
        int rx = x - pos.x;
        int rz = z - pos.y;
        bool light = Blocks::getBlockFromID(chunk->getBlock(rx, y, rz)).lightEmit > 0;
        printf("%d\n", light);
        chunk->removeBlock(rx, y, rz);
        // chunk->fullRebuildMesh();
        chunk->findMaxMin();
        chunk->recalculateLighting();
        bool left = rx == 0, right = rx == Chunk::chunkW-1, back = rz == 0, front = rz == Chunk::chunkL-1;
        if(left || right || front || back) {
            AdjChunks adj = getAdjacentChunks(pos);
            if((left || light) && adj.left) // Do same to rest
                adj.left->fullRebuildMesh();
            if(right && (adj.right || light))
                adj.right->fullRebuildMesh();
            if(front && (adj.front || light))
                adj.front->fullRebuildMesh();
            if(back && (adj.back || light))
                adj.back->fullRebuildMesh();
        }
        chunk->recalculateBleedLighting();
        chunk->rebuildMesh();
    }
    
    void removeBlock(const glm::ivec3 pos) {
        removeBlock(pos.x, pos.y, pos.z);
    }
    
    void removeBlock(const int x, const int y, const int z) {
        Chunk *chunk = getChunk(x, z);
        if(chunk == nullptr) return;
        removeBlock(chunk, x, y, z);
    }
    
    AdjChunks getAdjacentChunks(const glm::ivec2 pos) {
        AdjChunks adj { nullptr };
        if(auto it = chunks.find(generateChunkKey({pos.x, pos.y + Chunk::chunkL})); it != chunks.end()) {
            adj.front = &it->second;
        }
        if(auto it = chunks.find(generateChunkKey({pos.x, pos.y - Chunk::chunkL})); it != chunks.end()) {
            adj.back = &it->second;
        }
        if(auto it = chunks.find(generateChunkKey({pos.x + Chunk::chunkW, pos.y})); it != chunks.end()) {
            adj.right = &it->second;
        }
        if(auto it = chunks.find(generateChunkKey({pos.x - Chunk::chunkW, pos.y})); it != chunks.end()) {
            adj.left = &it->second;
        }
        return adj;
    }
}
