#include "world/World.h"

namespace World {
    std::vector<Chunk> chunks;
    std::mutex chunkMutex;
    BlockID getBlock(int x, int y, int z) {
        int chunkX = x - x%16;
        int chunkZ = z - z%16;
        glm::ivec2 chunkPos{chunkX, chunkZ};
        
        for(auto &c : chunks) {
            if(c.getPos() == chunkPos) {
                return c.getBlock(x%16, y, z%16);
            }
        }
        return Blocks::nullBlockID;
    }
    
    void addBlock(BlockID id, int x, int y, int z) {
        int chunkX = x - x%16;
        int chunkZ = z - z%16;
        printf("%d %d\n", x, z);
        glm::ivec2 chunkPos{chunkX, chunkZ};
        
        for(auto &c : chunks) {
            if(c.getPos() == chunkPos) {
                printf("Added block\n");
                int rx = x % Chunk::chunkW;
                int rz = z % Chunk::chunkL;
                c.addBlock(id, rx, y, rz);
                c.rebuildMesh();
                bool left = rx == 0, right = rx == Chunk::chunkW-1, back = rz == 0, front = rz == Chunk::chunkL-1;
                if(left || right || front || back) {
                    AdjChunks adj = World::getAdjacentChunks(chunkPos);
                    if(left)
                        adj.left->rebuildMesh();
                    if(right)
                        adj.right->rebuildMesh();
                    if(front)
                        adj.front->rebuildMesh();
                    if(back)
                        adj.back->rebuildMesh();
                }
                return;
            }
        }
    }
    
    void removeBlock(int x, int y, int z) {
        int chunkX = x - x%16;
        int chunkZ = z - z%16;
        glm::ivec2 chunkPos{chunkX, chunkZ};
        printf("%d %d\n", x, z);
        
        for(auto &c : chunks) {
            if(c.getPos() == chunkPos) {
                printf("Removed block\n");
                int rx = x % Chunk::chunkW;
                int rz = z % Chunk::chunkL;
                c.removeBlock(rx, y, rz);
                c.rebuildMesh();
                bool left = rx == 0, right = rx == Chunk::chunkW-1, back = rz == 0, front = rz == Chunk::chunkL-1;
                if(left || right || front || back) {
                    AdjChunks adj = World::getAdjacentChunks(chunkPos);
                    if(left)
                        adj.left->rebuildMesh();
                    if(right)
                        adj.right->rebuildMesh();
                    if(front)
                        adj.front->rebuildMesh();
                    if(back)
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
        AdjChunks adj { 0 };
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
