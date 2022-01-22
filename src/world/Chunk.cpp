#include <mutex>
#include "world/Chunk.h"
#include "glm/gtc/noise.hpp"
#include "renderer/texture2D.h"
#include "renderer/renderer.h"
#include "utils/DebugStats.h"
#include "world/World.h"
#include "glm/gtc/random.hpp"

Chunk::Chunk(int xx, int yy) : pos(xx*chunkW,yy*chunkL) {
    maxY = 0;
    minY = Chunk::chunkH;
    for(int y = 0; y < chunkH; y++)
        for(int x = 0; x < chunkW; x++)
            for(int z = 0; z < chunkL; z++)
                blocks[y][x][z] = 0;
    status = EMPTY;
}

Chunk::Chunk() {
    
}

Chunk::~Chunk() {
    
}

void Chunk::show() {
    if(status == HIDDEN) {
        status = SHOWING;
    }
}
void Chunk::hide() {
    if(status == SHOWING) {
        status = HIDDEN;
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

float Chunk::getNoise(glm::vec2 position) {
    return getNoise(position.x, position.y);
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
    const BlockID grassID = Blocks::getIdFromName("Grass"),
        dirtID = Blocks::getIdFromName("Dirt"),
        stoneID = Blocks::getIdFromName("Stone"),
        sandID = Blocks::getIdFromName("Sand"),
        waterID = Blocks::getIdFromName("Water"),
        logID = Blocks::getIdFromName("Log"),
        leavesID = Blocks::getIdFromName("Leaves"),
        bedrockID = Blocks::getIdFromName("Bedrock");
    
    int height, stoneHeight;
    bool tree;
    status = BUILDING;
    for(int x = 0; x < chunkW; x++) {
        for(int z = 0; z < chunkL; z++) {
            height = getNoise(pos.x + x, pos.y + z)+70;
            stoneHeight = (float)height / 1.15f;
            tree = glm::linearRand(0.f, 1.f) > .99f;
            
            blocks[0][x][z] = bedrockID;
            for(int y = 1; y < height; y++) {
                if(y < stoneHeight-1)
                    blocks[y][x][z] = stoneID;
                else if(y < height-1)
                    blocks[y][x][z] = dirtID;
                else if(y < 81) {
                    blocks[y][x][z] = sandID;
                    if(y < 80)
                        for(int w = y+1; w < 80; w++)
                            blocks[w][x][z] = waterID;
                } else
                    blocks[y][x][z] = grassID;
            }
            if(tree && height-1 >= 81 && x > 1 && x < chunkW-2 && z > 1 && z < chunkL-2) {
                int treeHeight = glm::linearRand(4, 7);
                if(blocks[height-1][x][z] == grassID)
                    blocks[height-1][x][z] = dirtID;
                for(int y = height; y < height+treeHeight; y++)
                    blocks[y][x][z] = logID;
                
                blocks[height+treeHeight][x][z] = leavesID;
                
                blocks[height+treeHeight][x-1][z] = leavesID;
                blocks[height+treeHeight][x+1][z] = leavesID;
                
                blocks[height+treeHeight][x][z-1] = leavesID;
                blocks[height+treeHeight][x][z+1] = leavesID;
                
                blocks[height+treeHeight-1][x-1][z] = leavesID;
                blocks[height+treeHeight-1][x+1][z] = leavesID;
                
                blocks[height+treeHeight-1][x-1][z+1] = leavesID;
                blocks[height+treeHeight-1][x][z+1] = leavesID;
                blocks[height+treeHeight-1][x+1][z+1] = leavesID;
                
                blocks[height+treeHeight-1][x-1][z-1] = leavesID;
                blocks[height+treeHeight-1][x][z-1] = leavesID;
                blocks[height+treeHeight-1][x+1][z-1] = leavesID;
                
                for(int yy=height+treeHeight-3; yy < height+treeHeight-1; yy++)
                    for(int xx=x-2; xx < x+3; xx++)
                        for(int zz = z-2; zz < z+3; zz++)
                            if(xx != x || zz != z)
                                blocks[yy][xx][zz] = leavesID;
            }
        }
    }
    status = BUILT;
}

void Chunk::rebuildMesh() {
    status = MESHING;
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
                    bool top = y == chunkH-1 ? true : !blocks[y+1][x][z],
                        bottom = y == 0 ? false : !blocks[y-1][x][z],
                        left = x == 0 ? chunks.left && !chunks.left->blocks[y][chunkW-1][z] : !blocks[y][x-1][z],
                        right = x == chunkW-1 ? chunks.right && !chunks.right->blocks[y][0][z] : !blocks[y][x+1][z],
                        front = z == chunkL-1 ? chunks.front && !chunks.front->blocks[y][x][0] : !blocks[y][x][z+1],
                        back = z == 0 ? chunks.back && !chunks.back->blocks[y][x][chunkL-1] : !blocks[y][x][z-1];
                    
                    if(top || bottom || left || right || front || back) {
                        if(y < minY) minY = y;
                        if(y > maxY) maxY = y;
                        if(b.liquid)
                            Renderer::generateCubeMesh(transparentMesh.v, pos.x + x, y-(1.f/16.f), pos.y + z, b.tex, top, bottom, left, right, front, back);
                        else
                            Renderer::generateCubeMesh(transparentMesh.v, pos.x + x, y, pos.y + z, b.tex, top, bottom, left, right, front, back);
                    }
                } else {
                    bool top = y == chunkH-1 ? true : Blocks::getBlockFromID(blocks[y+1][x][z]).transparent,
                        bottom = y == 0 ? false : Blocks::getBlockFromID(blocks[y-1][x][z]).transparent,
                        left = x == 0 ? chunks.left && Blocks::getBlockFromID(chunks.left->blocks[y][chunkW-1][z]).transparent : Blocks::getBlockFromID(blocks[y][x-1][z]).transparent,
                        right = x == chunkW-1 ? chunks.right && Blocks::getBlockFromID(chunks.right->blocks[y][0][z]).transparent : Blocks::getBlockFromID(blocks[y][x+1][z]).transparent,
                        front = z == chunkL-1 ? chunks.front && Blocks::getBlockFromID(chunks.front->blocks[y][x][0]).transparent : Blocks::getBlockFromID(blocks[y][x][z+1]).transparent,
                        back = z == 0 ? chunks.back && Blocks::getBlockFromID(chunks.back->blocks[y][x][chunkL-1]).transparent : Blocks::getBlockFromID(blocks[y][x][z-1]).transparent;
                    
                    if(top || bottom || left || right || front || back) {
                        if(y < minY) minY = y;
                        if(y > maxY) maxY = y;
                        Renderer::generateCubeMesh(mesh.v, pos.x + x, y, pos.y + z, b.tex, top, bottom, left, right, front, back);
                    }
                }
            }
        }
    }
    
    status = SHOWING;
}

bool Chunk::isVisible(const Frustum *f) const {
    return f->isBoxVisible(glm::vec3(pos.x, minY-1, pos.y), glm::vec3(pos.x + Chunk::chunkW, maxY+1, pos.y + Chunk::chunkL));
}

bool Chunk::operator<(const Chunk& other) const {
    if(status == HIDDEN) return false;
    if(other.status == HIDDEN) return true;
    auto cpos = glm::vec2(CameraConfig::cameraPos.x, CameraConfig::cameraPos.z);
    auto fpos = glm::vec2((float)this->pos.x, (float)this->pos.y) - cpos;
    auto ofpos = glm::vec2((float)other.pos.x, (float)other.pos.y) - cpos;
    return glm::dot(fpos, fpos) < glm::dot(ofpos, ofpos);
}
