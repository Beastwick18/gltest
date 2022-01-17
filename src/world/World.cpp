#include "world/World.h"

namespace World {
    std::map<std::string, Chunk> chunks;
    std::mutex chunkMutex;
    
    std::string generateChunkKey(const glm::ivec2 pos) {
        return std::to_string(pos.x) + "," + std::to_string(pos.y);
    }
    
    // Ray casting :D
    RaycastResults raycast(const glm::vec3 startPos, const glm::vec3 dir, const float length) {
        RaycastResults result;
        result.block.hit = false;
        result.liquid.blockID = Blocks::nullBlockID;
        result.block.blockID = Blocks::nullBlockID;
        
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
            result.liquid = {true, pos, {0, 0, 0}, blockID};
            blockID = Blocks::airBlockID;
        }
        
        while(currChunk && glm::distance(startPos, pos) <= length && blockID == Blocks::airBlockID && blockID != Blocks::nullBlockID) {
            min = glm::min(tMax.x, glm::min(tMax.y, tMax.z));
            for(int i = 0; i < tMax.length(); i++)
                if(tMax[i] == min) {
                    pos[i] += step[i];
                    tMax[i] += tDelta[i];
                    side = i;
                    break;
                }
            
            glm::ivec2 cpos { glm::floor(pos.x / Chunk::chunkW) * Chunk::chunkW, glm::floor(pos.z / Chunk::chunkL) * Chunk::chunkL};
            if(cpos != currChunk->getPos())
                currChunk = getChunk(cpos);
            blockID = getBlock(currChunk, pos.x, pos.y, pos.z);
            if(Blocks::getBlockFromID(blockID).liquid) {
                if(!result.liquid.hit) {
                    result.liquid = {true, pos, {0, 0, 0}, blockID};
                    result.liquid.hitSide[side] = -step[side];
                }
                blockID = Blocks::airBlockID;
            }
        }
        
        if(blockID != Blocks::nullBlockID && blockID != Blocks::airBlockID && !Blocks::getBlockFromID(blockID).liquid) {
            result.block = { true, pos, {0, 0, 0}, blockID };
            result.block.hitSide[side] = -step[side];
        }
        return result; 
    }
    
    Chunk *getChunk(const glm::ivec2 pos) {
        return getChunk(pos.x, pos.y);
    }
    
    Chunk *getChunk(const int x, const int z) {
        glm::ivec2 chunkPos{Chunk::chunkW * (x / Chunk::chunkW), Chunk::chunkL * (z / Chunk::chunkL)};
        
        auto i = chunks.find(generateChunkKey(chunkPos));
        if(i == chunks.end()) return nullptr;
        return &i->second;
    }
    
    BlockID getBlock(const Chunk *chunk, const glm::ivec3 pos) {
        return getBlock(chunk, pos.x, pos.y, pos.z);
    }
    
    BlockID getBlock(const Chunk *chunk, const int x, const int y, const int z) {
        if(chunk == nullptr) return Blocks::nullBlockID;
        
        const auto &pos = chunk->getPos();
        return chunk->getBlock(x - pos.x, y, z - pos.y);
    }
    
    BlockID getBlock(const glm::ivec3 pos) {
        return getBlock(pos.x, pos.y, pos.z);
    }
    
    BlockID getBlock(const int x, const int y, const int z) {
        auto chunk = getChunk(x, z);
        if(chunk == nullptr) return Blocks::nullBlockID;
        return getBlock(chunk, x, y, z);
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
        chunk->rebuildMesh();
        AdjChunks adj = getAdjacentChunks(pos);
        bool left = rx == 0, right = rx == Chunk::chunkW-1, back = rz == 0, front = rz == Chunk::chunkL-1;
        if(left || right || front || back) {
            AdjChunks adj = getAdjacentChunks(pos);
            if(left && adj.left)
                adj.left->rebuildMesh();
            if(right && adj.right)
                adj.right->rebuildMesh();
            if(front && adj.front)
                adj.front->rebuildMesh();
            if(back && adj.back)
                adj.back->rebuildMesh();
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
        chunk->removeBlock(rx, y, rz);
        chunk->rebuildMesh();
        AdjChunks adj = getAdjacentChunks(pos);
        bool left = rx == 0, right = rx == Chunk::chunkW-1, back = rz == 0, front = rz == Chunk::chunkL-1;
        if(left || right || front || back) {
            AdjChunks adj = getAdjacentChunks(pos);
            if(left && adj.left)
                adj.left->rebuildMesh();
            if(right && adj.right)
                adj.right->rebuildMesh();
            if(front && adj.front)
                adj.front->rebuildMesh();
            if(back && adj.back)
                adj.back->rebuildMesh();
        }
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
