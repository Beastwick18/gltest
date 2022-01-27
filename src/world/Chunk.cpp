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
            for(int z = 0; z < chunkL; z++) {
                blocks[y][x][z] = 0;
                skyLight[y][x][z] = 1;
                skyLightSpread[y][x][z] = 0;
            }
    status = EMPTY;
    mesh.init(chunkW * chunkH * chunkL / 2);
    transparentMesh.init(chunkW * chunkH * chunkL / 2);
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
                
                // blocks[height+treeHeight][x][z] = leavesID;
                addBlock(leavesID, x, height + treeHeight, z);
                
                // blocks[height+treeHeight][x-1][z] = leavesID;
                // blocks[height+treeHeight][x+1][z] = leavesID;
                addBlock(leavesID, x - 1, height + treeHeight, z);
                addBlock(leavesID, x + 1, height + treeHeight, z);
                
                // blocks[height+treeHeight][x][z-1] = leavesID;
                // blocks[height+treeHeight][x][z+1] = leavesID;
                addBlock(leavesID, x, height + treeHeight, z - 1);
                addBlock(leavesID, x, height + treeHeight, z + 1);
                
                // blocks[height+treeHeight-1][x-1][z] = leavesID;
                // blocks[height+treeHeight-1][x+1][z] = leavesID;
                addBlock(leavesID, x - 1, height + treeHeight - 1, z);
                addBlock(leavesID, x + 1, height + treeHeight - 1, z);
                
                // blocks[height+treeHeight-1][x-1][z+1] = leavesID;
                // blocks[height+treeHeight-1][x][z+1] = leavesID;
                // blocks[height+treeHeight-1][x+1][z+1] = leavesID;
                addBlock(leavesID, x - 1, height + treeHeight - 1, z + 1);
                addBlock(leavesID, x, height + treeHeight - 1, z + 1);
                addBlock(leavesID, x + 1, height + treeHeight - 1, z + 1);
                
                // blocks[height+treeHeight-1][x-1][z-1] = leavesID;
                // blocks[height+treeHeight-1][x][z-1] = leavesID;
                // blocks[height+treeHeight-1][x+1][z-1] = leavesID;
                addBlock(leavesID, x - 1, height + treeHeight - 1, z - 1);
                addBlock(leavesID, x, height + treeHeight - 1, z - 1);
                addBlock(leavesID, x + 1, height + treeHeight - 1, z - 1);
                
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

void Chunk::calculateSkyLighting(int x, int y, int z, float prev) {
    if(prev * 15 <= skyLight[y][x][z] || prev <= 0|| x < 0 || y < minY || z < 0 || x >= chunkW || y >= maxY+1 || z >= chunkL)
        return;
    
    skyLight[y][x][z] = glm::max(skyLight[y][x][z], (unsigned char)(prev*15));
    if(auto b = blocks[y][x][z]; b != Blocks::airBlockID && b != Blocks::nullBlockID) {
        // skyLight[y][x][z] = (unsigned char)( prev*15 );
        prev = prev - Blocks::getBlockFromID(b).lightBlocking;
        // skyLight[y][x][z] = prev*15;
        // calculateLighting(x, y, z, prev);
        if(prev <= 0) return;
    }
    calculateSkyLighting(x, y-1, z, prev);
    calculateSkyLightingSpread(x, y+1, z, prev);
    calculateSkyLightingSpread(x+1, y, z, prev);
    calculateSkyLightingSpread(x-1, y, z, prev);
    calculateSkyLightingSpread(x, y, z-1, prev);
    calculateSkyLightingSpread(x, y, z+1, prev);
}

void Chunk::calculateSkyLightingSpread(int x, int y, int z, float prev) {
    if(prev * 15 <= skyLightSpread[y][x][z] || prev <= 0 || x < 0 || y < minY || z < 0 || x >= chunkW || y >= maxY+1 || z >= chunkL)
        return;
    float lb = .2f;
    skyLightSpread[y][x][z] = glm::max(skyLightSpread[y][x][z], (unsigned char)(prev*15));
    if(auto b = blocks[y][x][z]; !Blocks::getBlockFromID(b).transparent && b != Blocks::nullBlockID) {
    // if(auto b = blocks[y][x][z]; b != Blocks::airBlockID && b != Blocks::nullBlockID) {
        // light[y][x][z] = (unsigned char)( prev*15 );
        // lb = glm::max(.2f, Blocks::getBlockFromID(b).lightBlocking);
    //     // prev = prev - 0.9f;
        // prev -= lb;
        return;
    }
    // light[y][x][z] = 15;
    calculateSkyLightingSpread(x, y+1, z, prev - lb);
    calculateSkyLightingSpread(x, y-1, z, prev - lb);
    calculateSkyLightingSpread(x-1, y, z, prev - lb);
    calculateSkyLightingSpread(x+1, y, z, prev - lb);
    calculateSkyLightingSpread(x, y, z-1, prev - lb);
    calculateSkyLightingSpread(x, y, z+1, prev - lb);

}

void Chunk::calculateLighting(int x, int y, int z, float prev) {
    if(prev * 15 <= light[y][x][z] || prev <= 0 || x < 0 || y < minY || z < 0 || x >= chunkW || y >= maxY+1 || z >= chunkL)
        return;
    float lb = .2f;
    light[y][x][z] = glm::max(light[y][x][z], (unsigned char)(prev*15));
    // if(auto b = blocks[y][x][z]; b != Blocks::airBlockID && b != Blocks::nullBlockID) {
    // if(auto b = blocks[y][x][z]; !Blocks::getBlockFromID(b).transparent && b != Blocks::nullBlockID) {
        // light[y][x][z] = (unsigned char)( prev*15 );
        // lb = Blocks::getBlockFromID(b).lightBlocking;
    //     // prev = prev - 0.9f;
        // return;
    // }
    // light[y][x][z] = 15;
    calculateLighting(x, y+1, z, prev-lb);
    calculateLighting(x, y-1, z, prev-lb);
    calculateLighting(x-1, y, z, prev-lb);
    calculateLighting(x+1, y, z, prev-lb);
    calculateLighting(x, y, z-1, prev-lb);
    calculateLighting(x, y, z+1, prev-lb);
}

// TODO: Either create a vector of Lights that contains all the lights that spread past the edge of the chunk, or
// make a way to grab a light value and start another calculateSkyLighting/calculateLighting from that position.
// TODO: Maybe have multiple meshes so that we only have to rebuild a portion of the chunk, as opposed to meshing
// the entire chunk. This may improve lighting performance.
// TODO: Maybe change lighting to per vertex. The way this would be done is to have the light arrays represent the
// light value at each possible vertex. Each chunk would not calculate the vertex that is on the far edge of the chunk
// border. This value is left to the chunk that shares that edge.
// The other alternative would be to store a struct containing the light values for the top face, left face, and front face.
// You could use the bottom, right, and back face too. Because two faces in the mesh wont ever be rendered on top of eachother,
// you can assume the top of the block below you is the same as your bottom face.
void Chunk::rebuildMesh() {
    status = MESHING;
    AdjChunks chunks = World::getAdjacentChunks(pos);
    // mesh.v.clear();
    // transparentMesh.v.clear();
    mesh.clear();
    transparentMesh.clear();
    std::vector<Light> lights;
    bool top, bottom, left, right, front, back;
    for(int y = 0; y < chunkH; y++)
        for(int x = 0; x < chunkW; x++)
            for(int z = 0; z < chunkL; z++) {
                light[y][x][z] = skyLight[y][x][z] = skyLightSpread[y][x][z] = 0;
                BlockID id = blocks[y][x][z];
                if(id == Blocks::airBlockID || id == Blocks::nullBlockID)
                    continue;
                const auto &b = Blocks::getBlockFromID(id);
                if(b.transparent) {
                    top = y == chunkH-1 ? true : !blocks[y+1][x][z],
                    bottom = y == 0 ? false : !blocks[y-1][x][z],
                    left = x == 0 ? chunks.left && !chunks.left->blocks[y][chunkW-1][z] : !blocks[y][x-1][z],
                    right = x == chunkW-1 ? chunks.right && !chunks.right->blocks[y][0][z] : !blocks[y][x+1][z],
                    front = z == chunkL-1 ? chunks.front && !chunks.front->blocks[y][x][0] : !blocks[y][x][z+1],
                    back = z == 0 ? chunks.back && !chunks.back->blocks[y][x][chunkL-1] : !blocks[y][x][z-1];
                } else {
                    top = y == chunkH-1 ? true : Blocks::getBlockFromID(blocks[y+1][x][z]).transparent,
                    bottom = y == 0 ? false : Blocks::getBlockFromID(blocks[y-1][x][z]).transparent,
                    left = x == 0 ? chunks.left && Blocks::getBlockFromID(chunks.left->blocks[y][chunkW-1][z]).transparent : Blocks::getBlockFromID(blocks[y][x-1][z]).transparent,
                    right = x == chunkW-1 ? chunks.right && Blocks::getBlockFromID(chunks.right->blocks[y][0][z]).transparent : Blocks::getBlockFromID(blocks[y][x+1][z]).transparent,
                    front = z == chunkL-1 ? chunks.front && Blocks::getBlockFromID(chunks.front->blocks[y][x][0]).transparent : Blocks::getBlockFromID(blocks[y][x][z+1]).transparent,
                    back = z == 0 ? chunks.back && Blocks::getBlockFromID(chunks.back->blocks[y][x][chunkL-1]).transparent : Blocks::getBlockFromID(blocks[y][x][z-1]).transparent;
                }
                if(top || bottom || left || right || front || back) {
                    if(y < minY) minY = y;
                    if(y > maxY) maxY = y;
                    
                    if(b.lightEmit > 0.f) {
                        lights.push_back({{x, y, z}, b.lightEmit});
                    }
                }
            }
    // printf("%d %d\n", maxY, minY);
    for(int x = 0; x < chunkW; x++)
        for(int z = 0; z < chunkL; z++) {
            calculateSkyLighting(x, maxY, z, 1.f);
            // for(int y = 0; y < chunkH; y++) {
            //     const BlockID id = blocks[y][x][z];
            //     if(Blocks::getBlockFromID(id).name == "Lava") {
            //         calculateLighting(x, y, z, 1.f);
            //     }
            // }
        }
    // if(chunks.left) {
    //     for(int z = 0; z < chunkL; z++) {
    //         for(int y = 0; y < chunkH; y++) {
    //             if(chunks.left.skyLight[y][0][z] != 0)
                    
    //         }
    //     }
    // }
    for(const auto &p : lights) {
        calculateLighting(p.pos.x, p.pos.y, p.pos.z, p.val);
    }
    
    for(int x = 0; x < chunkW; x++) {
        for(int z = 0; z < chunkL; z++) {
            for(int y = minY; y <= maxY; y++) {
                const BlockID id = blocks[y][x][z];
                if(id == Blocks::airBlockID || id == Blocks::nullBlockID)
                    continue;
                
                const Block b = Blocks::getBlockFromID(blocks[y][x][z]);
                // const float l = (float)(1+light[y][x][z])/16.f;
                const float l = (float)(1+light[y][x][z])/16.f;
                const float sl = (float)(1+glm::max(skyLight[y][x][z], skyLightSpread[y][x][z]))/16.f;
                // const float l = Blocks::getBlockFromID(id).lightBlocking;
                
                if(b.transparent) {
                    top = y == chunkH-1 || !blocks[y+1][x][z];
                    bottom = y != 0 && !blocks[y-1][x][z];
                    left = x == 0 ? chunks.left && !chunks.left->blocks[y][chunkW-1][z] : !blocks[y][x-1][z];
                    right = x == chunkW-1 ? chunks.right && !chunks.right->blocks[y][0][z] : !blocks[y][x+1][z];
                    front = z == chunkL-1 ? chunks.front && !chunks.front->blocks[y][x][0] : !blocks[y][x][z+1];
                    back = z == 0 ? chunks.back && !chunks.back->blocks[y][x][chunkL-1] : !blocks[y][x][z-1];
                    
                    if(top || bottom || left || right || front || back) {
                        if(b.liquid)
                            Renderer::generateCubeMesh(transparentMesh, pos.x + x, y-(1.f/16.f), pos.y + z, b.tex, top, bottom, left, right, front, back, l, sl);
                        else
                            Renderer::generateCubeMesh(transparentMesh, pos.x + x, y, pos.y + z, b.tex, top, bottom, left, right, front, back, l, sl);
                    }
                } else {
                    top = y == chunkH-1 || Blocks::getBlockFromID(blocks[y+1][x][z]).transparent;
                    bottom = y != 0 && Blocks::getBlockFromID(blocks[y-1][x][z]).transparent;
                    left = x == 0 ? chunks.left && Blocks::getBlockFromID(chunks.left->blocks[y][chunkW-1][z]).transparent : Blocks::getBlockFromID(blocks[y][x-1][z]).transparent;
                    right = x == chunkW-1 ? chunks.right && Blocks::getBlockFromID(chunks.right->blocks[y][0][z]).transparent : Blocks::getBlockFromID(blocks[y][x+1][z]).transparent;
                    front = z == chunkL-1 ? chunks.front && Blocks::getBlockFromID(chunks.front->blocks[y][x][0]).transparent : Blocks::getBlockFromID(blocks[y][x][z+1]).transparent;
                    back = z == 0 ? chunks.back && Blocks::getBlockFromID(chunks.back->blocks[y][x][chunkL-1]).transparent : Blocks::getBlockFromID(blocks[y][x][z-1]).transparent;
                    
                    if(top || bottom || left || right || front || back) {
                        Renderer::generateCubeMesh(mesh, pos.x + x, y, pos.y + z, b.tex, top, bottom, left, right, front, back, l, sl);
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
