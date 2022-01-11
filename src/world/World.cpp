#include "world/World.h"

namespace World {
    std::vector<Chunk> chunks;
    std::mutex chunkMutex;
    
    BlockID getBlock(glm::ivec3 pos) {
        return getBlock(pos.x, pos.y, pos.z);
    }
    
    BlockID getBlock(int x, int y, int z) {
        int rx = x%Chunk::chunkW;
        int rz = z%Chunk::chunkL;
        glm::ivec2 chunkPos{x - rx, z - rz};
        
        for(auto &c : chunks)
            if(c.getPos() == chunkPos)
                return c.getBlock(rx, y, rz);
        
        return Blocks::nullBlockID;
    }
    
    void addBlock(BlockID id, glm::ivec3 pos) {
        addBlock(id, pos.x, pos.y, pos.z);
    }
    
    void addBlock(BlockID id, int x, int y, int z) {
        int rx = x % Chunk::chunkW;
        int rz = z % Chunk::chunkL;
        glm::ivec2 chunkPos{x - rx, z - rz};
        
        for(auto &c : chunks) {
            if(c.getPos() == chunkPos) {
                c.addBlock(id, rx, y, rz);
                c.rebuildMesh();
                bool left = rx == 0, right = rx == Chunk::chunkW-1, back = rz == 0, front = rz == Chunk::chunkL-1;
                if(left || right || front || back) {
                    AdjChunks adj = World::getAdjacentChunks(chunkPos);
                    if(left && adj.left)
                        adj.left->rebuildMesh();
                    if(right && adj.right)
                        adj.right->rebuildMesh();
                    if(front && adj.front)
                        adj.front->rebuildMesh();
                    if(back && adj.back)
                        adj.back->rebuildMesh();
                }
                return;
            }
        }
    }
    
    void removeBlock(glm::ivec3 pos) {
        removeBlock(pos.x, pos.y, pos.z);
    }
    
    void removeBlock(int x, int y, int z) {
        int rx = x % Chunk::chunkW;
        int rz = z % Chunk::chunkL;
        glm::ivec2 chunkPos{x - rx, z - rz};
        
        for(auto &c : chunks) {
            if(c.getPos() == chunkPos) {
                c.removeBlock(rx, y, rz);
                c.rebuildMesh();
                bool left = rx == 0, right = rx == Chunk::chunkW-1, back = rz == 0, front = rz == Chunk::chunkL-1;
                if(left || right || front || back) {
                    AdjChunks adj = World::getAdjacentChunks(chunkPos);
                    if(left && adj.left)
                        adj.left->rebuildMesh();
                    if(right && adj.right)
                        adj.right->rebuildMesh();
                    if(front && adj.front)
                        adj.front->rebuildMesh();
                    if(back && adj.back)
                        adj.back->rebuildMesh();
                }
                return;
            }
        }
    }
    
    AdjChunks getAdjacentChunks(glm::ivec2 pos) {
        glm::ivec2 leftPos  {pos.x-Chunk::chunkW, pos.y};
        glm::ivec2 rightPos {pos.x+Chunk::chunkW, pos.y};
        glm::ivec2 frontPos {pos.x, pos.y+Chunk::chunkL};
        glm::ivec2 backPos  {pos.x, pos.y-Chunk::chunkL};
        AdjChunks adj { nullptr };
        for(auto &c : chunks) {
            if(c.getPos() == leftPos)
                adj.left = &c;
            else if(c.getPos() == rightPos)
                adj.right = &c;
            else if(c.getPos() == frontPos)
                adj.front= &c;
            else if(c.getPos() == backPos)
                adj.back = &c;
        }
        return adj;
    }
}
