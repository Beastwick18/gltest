// TODO: Create a chunk mesh of vertices that should be sent to the renderer for rendering. Only update the mesh when something changes

#include <mutex>
#include "world/Chunk.h"
#include "glm/gtc/noise.hpp"
#include "renderer/texture2D.h"
#include "renderer/renderer.h"
#include "utils/DebugStats.h"
#include "world/World.h"
#include "glm/gtc/random.hpp"

Chunk::Chunk(int xx, int yy) : pos(xx*chunkW,yy*chunkL) {
    for(int y = 0; y < chunkH; y++)
        for(int x = 0; x < chunkW; x++)
            for(int z = 0; z < chunkL; z++)
                blocks[y][x][z] = 0;
    status = ChunkStatus::EMPTY;
}

Chunk::~Chunk() {
    
}

void Chunk::show() {
    if(status == ChunkStatus::HIDDEN) {
        status = ChunkStatus::SHOWING;
    }
}
void Chunk::hide() {
    if(status == ChunkStatus::SHOWING) {
        status = ChunkStatus::HIDDEN;
    }
}

BlockID Chunk::getBlock(int x, int y, int z) const {
    if(x > chunkW-1 || z > chunkL-1 || y > chunkH-1 || x < 0 || y < 0 || z < 0)
        return Blocks::nullBlockID;
    
    return blocks[y][x][z];
}

void Chunk::addBlock(BlockID id, int x, int y, int z) {
    if(x < 0 || x >= chunkW || y < 0 || y >= chunkH || z < 0 || z >= chunkL)
        return;
    if(blocks[y][x][z] && !Blocks::getBlockFromID(blocks[y][x][z]).liquid)
        return;
    blocks[y][x][z] = id;
}

void Chunk::removeBlock(int x, int y, int z) {
    if(x < 0 || x >= chunkW || y < 0 || y >= chunkH || z < 0 || z >= chunkL)
        return;
    blocks[y][x][z] = Blocks::airBlockID;
}

float Chunk::getNoise(float x, float z) {
    glm::vec2 position((float)x/100.f, (float)z/100.f);
    float h = (1+glm::simplex(position))/3.0 * 50;
    
    float mid = (glm::simplex(glm::vec2(x/20.f,z/20.f)))/2.0 * 5;
    h += mid;
    
    float ext = (glm::simplex(glm::vec2(x/10.f, z/10.f))) * 1.f;
    h += ext;
    
    return h;
}

float caveTest(float x, float y, float z) {
    return (1.f+glm::simplex(glm::vec3(x,y,z)))/2.f;
}

void Chunk::generateChunk() {
    status = ChunkStatus::BUILDING;
    for(int x = 0; x < chunkW; x++) {
        for(int z = 0; z < chunkL; z++) {
            int height = getNoise(pos.x + x, pos.y + z)+70;
            float stoneHeight = (float)height / 1.15f;
            bool tree = glm::linearRand(0.f, 1.f) > .99f;
            for(int y = 0; y < height; y++) {
                if(y < stoneHeight-1)
                    blocks[y][x][z] = 3;
                else if(y < height-1)
                    blocks[y][x][z] = 2;
                else if(y < 81) {
                    blocks[y][x][z] = 6;
                    if(y < 80)
                        for(int w = y+1; w < 80; w++)
                            blocks[w][x][z] = 7;
                } else
                    blocks[y][x][z] = 1;
            }
            if(tree && height-1 >= 81 && x > 1 && x < chunkW-2 && z > 1 && z < chunkL-2) {
                int treeHeight = glm::linearRand(4, 7);
                if(blocks[height-1][x][z] == 1)
                    blocks[height-1][x][z] = 2;
                for(int y = height; y < height+treeHeight; y++) {
                    blocks[y][x][z] = 4;
                }
                blocks[height+treeHeight][x][z] = 5;
                
                blocks[height+treeHeight][x-1][z] = 5;
                blocks[height+treeHeight][x+1][z] = 5;
                
                blocks[height+treeHeight][x][z-1] = 5;
                blocks[height+treeHeight][x][z+1] = 5;
                
                blocks[height+treeHeight-1][x-1][z] = 5;
                blocks[height+treeHeight-1][x+1][z] = 5;
                
                blocks[height+treeHeight-1][x-1][z+1] = 5;
                blocks[height+treeHeight-1][x][z+1] = 5;
                blocks[height+treeHeight-1][x+1][z+1] = 5;
                
                blocks[height+treeHeight-1][x-1][z-1] = 5;
                blocks[height+treeHeight-1][x][z-1] = 5;
                blocks[height+treeHeight-1][x+1][z-1] = 5;
                
                for(int yy=height+treeHeight-3; yy < height+treeHeight-1; yy++)
                    for(int xx=x-2; xx < x+3; xx++)
                        for(int zz = z-2; zz < z+3; zz++)
                            if(xx != x || zz != z)
                                blocks[yy][xx][zz] = 5;
            }
        }
    }
    status = ChunkStatus::BUILT;
}

void Chunk::rebuildMesh() {
    // if(status != ChunkStatus::HIDDEN) return;
    status = ChunkStatus::MESHING;
    AdjChunks chunks = World::getAdjacentChunks(pos);
    mesh.v.clear();
    transparentMesh.v.clear();
    for(int y = 0; y < chunkH; y++) {
        for(int x = 0; x < chunkW; x++) {
            for(int z = 0; z < chunkL; z++) {
                BlockID id = blocks[y][x][z];
                if(id == Blocks::airBlockID || id == Blocks::nullBlockID)
                    continue;
                
                const Block b = Blocks::getBlockFromID(blocks[y][x][z]);
                
                if(b.transparent) {
                    bool top = y == chunkH-1 ? false : blocks[y+1][x][z];
                    bool bottom = y == 0 ? true : blocks[y-1][x][z];
                    bool left = x == 0 ? !chunks.left || chunks.left->blocks[y][chunkW-1][z] : blocks[y][x-1][z];
                    bool right = x == chunkW-1 ? !chunks.right || chunks.right->blocks[y][0][z] : blocks[y][x+1][z];
                    bool front = z == chunkL-1 ? !chunks.front || chunks.front->blocks[y][x][0] : blocks[y][x][z+1];
                    bool back = z == 0 ? !chunks.back || chunks.back->blocks[y][x][chunkL-1] : blocks[y][x][z-1];
                    
                    if(b.liquid) {
                        Renderer::generateCubeMesh(transparentMesh.v, pos.x + x, y-(1.f/16.f), pos.y + z, b.tex, !top, !bottom, !left, !right, !front, !back);
                    }else
                        Renderer::generateCubeMesh(transparentMesh.v, pos.x + x, y, pos.y + z, b.tex, !top, !bottom, !left, !right, !front, !back);
                } else {
                    bool top = y == chunkH-1 ? false : !Blocks::getBlockFromID(blocks[y+1][x][z]).transparent;
                    bool bottom = y == 0 ? true : !Blocks::getBlockFromID(blocks[y-1][x][z]).transparent;
                    bool left = x == 0 ? !chunks.left || !Blocks::getBlockFromID(chunks.left->blocks[y][chunkW-1][z]).transparent : !Blocks::getBlockFromID(blocks[y][x-1][z]).transparent;
                    bool right = x == chunkW-1 ? !chunks.right || !Blocks::getBlockFromID(chunks.right->blocks[y][0][z]).transparent : !Blocks::getBlockFromID(blocks[y][x+1][z]).transparent;
                    bool front = z == chunkL-1 ? !chunks.front || !Blocks::getBlockFromID(chunks.front->blocks[y][x][0]).transparent : !Blocks::getBlockFromID(blocks[y][x][z+1]).transparent;
                    bool back = z == 0 ? !chunks.back || !Blocks::getBlockFromID(chunks.back->blocks[y][x][chunkL-1]).transparent : !Blocks::getBlockFromID(blocks[y][x][z-1]).transparent;
                    Renderer::generateCubeMesh(mesh.v, pos.x + x, y, pos.y + z, b.tex, !top, !bottom, !left, !right, !front, !back);
                }
            }
        }
    }
    
    status = ChunkStatus::SHOWING;
}

bool Chunk::operator<(const Chunk& other) const {
    if(status == ChunkStatus::HIDDEN) return false;
    if(other.status == ChunkStatus::HIDDEN) return true;
    auto cpos = glm::vec2(CameraConfig::cameraPos.x, CameraConfig::cameraPos.z);
    auto fpos = glm::vec2((float)this->pos.x, (float)this->pos.y) - cpos;
    auto ofpos = glm::vec2((float)other.pos.x, (float)other.pos.y) - cpos;
    return glm::dot(fpos, fpos) < glm::dot(ofpos, ofpos);
}

// TODO: Maybe create maxNonAir, which contains the max y value of the chunk that contains a non air block. This could be used to not render chunks that only have blocks below the cameras pov
// Or maybe just split the chunk into 32x32x32 sub-chunks;
