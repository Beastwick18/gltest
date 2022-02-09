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
                
                addBlock(leavesID, x, height + treeHeight, z);
                
                addBlock(leavesID, x - 1, height + treeHeight, z);
                addBlock(leavesID, x + 1, height + treeHeight, z);
                
                addBlock(leavesID, x, height + treeHeight, z - 1);
                addBlock(leavesID, x, height + treeHeight, z + 1);
                
                addBlock(leavesID, x - 1, height + treeHeight - 1, z);
                addBlock(leavesID, x + 1, height + treeHeight - 1, z);
                
                addBlock(leavesID, x - 1, height + treeHeight - 1, z + 1);
                addBlock(leavesID, x, height + treeHeight - 1, z + 1);
                addBlock(leavesID, x + 1, height + treeHeight - 1, z + 1);
                
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
    if(prev * 15 <= skyLight[y][x][z] || prev <= 0|| x < 0 || y < minY || z < 0 || x >= chunkW || y >= maxY+2 || z >= chunkL)
        return;
    
    float lb = .2f;
    skyLight[y][x][z] = glm::max(skyLight[y][x][z], (unsigned char)(prev*15));
    if(auto b = blocks[y][x][z]; b != Blocks::airBlockID && b != Blocks::nullBlockID) {
        // skyLight[y][x][z] = (unsigned char)( prev*15 );
        prev = prev - Blocks::getBlockFromID(b).lightBlocking;
        // skyLight[y][x][z] = prev*15;
        // calculateLighting(x, y, z, prev);
        if(prev <= 0) return;
    }
    calculateSkyLighting(x, y-1, z, prev);
    calculateSkyLightingSpread(x, y+1, z, prev - lb);
    calculateSkyLightingSpread(x-1, y, z, prev - lb);
    calculateSkyLightingSpread(x+1, y, z, prev - lb);
    calculateSkyLightingSpread(x, y, z-1, prev - lb);
    calculateSkyLightingSpread(x, y, z+1, prev - lb);
}

void Chunk::calculateSkyLightingSpread(int x, int y, int z, float prev) {
    if(prev * 15 <= skyLightSpread[y][x][z] || prev <= 0 || x < 0 || y < minY || z < 0 || x >= chunkW || y >= maxY+1 || z >= chunkL)
        return;
    float lb = .2f;
    skyLightSpread[y][x][z] = glm::max(skyLightSpread[y][x][z], (unsigned char)(prev*15));
    if(const auto &b = blocks[y][x][z]; !Blocks::getBlockFromID(b).transparent && b != Blocks::nullBlockID) {
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
    float lb = .15f;
    light[y][x][z] = glm::max(light[y][x][z], (unsigned char)(prev*15));
    // if(auto b = blocks[y][x][z]; b != Blocks::airBlockID && b != Blocks::nullBlockID) {
    if(const auto &b = blocks[y][x][z]; !Blocks::getBlockFromID(b).transparent && b != Blocks::nullBlockID && prev != 1.f) {
        // light[y][x][z] = (unsigned char)( prev*15 );
        // lb = Blocks::getBlockFromID(b).lightBlocking;
    //     // prev = prev - 0.9f;
        return;
    }
    // light[y][x][z] = 15;
    calculateLighting(x, y+1, z, prev-lb);
    calculateLighting(x, y-1, z, prev-lb);
    calculateLighting(x+1, y, z, prev-lb);
    calculateLighting(x-1, y, z, prev-lb);
    calculateLighting(x, y, z+1, prev-lb);
    calculateLighting(x, y, z-1, prev-lb);
}

void Chunk::generateQuadMesh(Mesh<Vertex> &mesh, Vertex v0, Vertex v1, Vertex v2, Vertex v3) {
    mesh.addVertex(v0);
    mesh.addVertex(v1);
    mesh.addVertex(v2);
    mesh.addVertex(v2);
    mesh.addVertex(v3);
    mesh.addVertex(v0);
}

void Chunk::generateCubeMesh(Mesh<Vertex> &mesh, int cx, int y, int cz, BlockTexture tex, AdjChunks chunks, SurroundingBlocks adj) {
    float x = pos.x + cx;
    float z = pos.y + cz;
    if(adj.front) {
        float light = getLight(cx, y, cz+1);
        float skyLight = getSkyLight(cx, y, cz+1);
        generateQuadMesh(mesh,
            { {x,   y+1, z+1}, {0, 0, -1}, {tex.front.x, tex.front.y+tex.front.h}, light, skyLight },
            { {x,   y,   z+1}, {0, 0, -1}, {tex.front.x, tex.front.y}, light, skyLight },
            { {x+1, y,   z+1}, {0, 0, -1}, {tex.front.x+tex.front.w, tex.front.y}, light, skyLight },
            { {x+1, y+1, z+1}, {0, 0, -1}, {tex.front.x+tex.front.w, tex.front.y+tex.front.h}, light, skyLight }
        );
        DebugStats::triCount += 1;
    }
    
    if(adj.right) {
        float light = getLight(cx+1, y, cz);
        float skyLight = getSkyLight(cx+1, y, cz);
        generateQuadMesh(mesh,
            { {x+1, y+1, z+1}, {-1, 0, 0}, {tex.left.x, tex.left.y+tex.left.h}, light, skyLight },
            { {x+1, y,   z+1}, {-1, 0, 0}, {tex.left.x, tex.left.y}, light, skyLight },
            { {x+1, y,   z},   {-1, 0, 0}, {tex.left.x+tex.left.w, tex.left.y}, light, skyLight },
            { {x+1, y+1, z},   {-1, 0, 0}, {tex.left.x+tex.left.w, tex.left.y+tex.left.h}, light, skyLight }
        );
        DebugStats::triCount += 1;
    }
    
    if(adj.back) {
        float light = getLight(cx, y, cz-1);
        float skyLight = getSkyLight(cx, y, cz-1);
        generateQuadMesh(mesh,
                { {x,   y+1, z},   {0, 0, 1}, {tex.back.x+tex.back.w, tex.back.y+tex.back.h}, light, skyLight },
                { {x+1, y+1, z},   {0, 0, 1}, {tex.back.x, tex.back.y+tex.back.h}, light, skyLight },
                { {x+1, y,   z},   {0, 0, 1}, {tex.back.x, tex.back.y}, light, skyLight },
                { {x,   y,   z},   {0, 0, 1}, {tex.back.x+tex.back.w, tex.back.y}, light, skyLight }
        );
        DebugStats::triCount += 1;
    }
    
    if(adj.left) {
        float light = getLight(cx-1, y, cz);
        float skyLight = getSkyLight(cx-1, y, cz);
        generateQuadMesh(mesh,
                { {x, y+1, z+1},   {1, 0, 0}, {tex.right.x+tex.right.w, tex.right.y+tex.right.h}, light, skyLight },
                { {x, y+1, z},     {1, 0, 0}, {tex.right.x, tex.right.y+tex.right.h}, light, skyLight },
                { {x, y,   z},     {1, 0, 0}, {tex.right.x, tex.right.y}, light, skyLight },
                { {x, y,   z+1},   {1, 0, 0}, {tex.right.x+tex.right.w, tex.right.y}, light, skyLight }
        );
        DebugStats::triCount += 1;
    }
    
    if(adj.top) {
        float light = getLight(cx, y+1, cz);
        float skyLight = getSkyLight(cx, y+1, cz);
        generateQuadMesh(mesh,
                { {x+1, y+1, z},   {0, 1, 0}, {tex.top.x+tex.top.w, tex.top.y}, light, skyLight },
                { {x,   y+1, z},   {0, 1, 0}, {tex.top.x, tex.top.y}, light, skyLight },
                { {x,   y+1, z+1}, {0, 1, 0}, {tex.top.x, tex.top.y+tex.top.h}, light, skyLight },
                { {x+1, y+1, z+1}, {0, 1, 0}, {tex.top.x+tex.top.w, tex.top.y+tex.top.h}, light, skyLight }
        );
        DebugStats::triCount += 1;
    }
    
    if(adj.bottom) {
        float light = getLight(cx, y-1, cz);
        float skyLight = getSkyLight(cx, y-1, cz);
        generateQuadMesh(mesh,
                { {x,   y, z+1}, {0, -1, 0}, {tex.bottom.x, tex.bottom.y+tex.bottom.h}, light, skyLight },
                { {x,   y, z},   {0, -1, 0}, {tex.bottom.x, tex.bottom.y}, light, skyLight },
                { {x+1, y, z},   {0, -1, 0}, {tex.bottom.x+tex.bottom.w, tex.bottom.y}, light, skyLight },
                { {x+1, y, z+1}, {0, -1, 0}, {tex.bottom.x+tex.bottom.w, tex.bottom.y+tex.bottom.h}, light, skyLight }
        );
        DebugStats::triCount += 1;
    }
    
}

void Chunk::generateLiquidMesh(Mesh<Vertex> &mesh, int cx, int y, int cz, BlockTexture tex, AdjChunks chunks, SurroundingBlocks adj) {
    float x = pos.x + cx;
    float z = pos.y + cz;
    float h = 1.f;
    if(adj.top) {
        float light = getLight(cx, y+1, cz);
        float skyLight = getSkyLight(cx, y+1, cz);
        tex.left.h *= 15.f/16.f;
        tex.right.h *= 15.f/16.f;
        tex.front.h *= 15.f/16.f;
        tex.back.h *= 15.f/16.f;
        h = 15.f/16.f;
        generateQuadMesh(mesh,
                { {x+1, y+h, z},   {0, 1, 0}, {tex.top.x+tex.top.w, tex.top.y}, light, skyLight },
                { {x,   y+h, z},   {0, 1, 0}, {tex.top.x, tex.top.y}, light, skyLight },
                { {x,   y+h, z+1}, {0, 1, 0}, {tex.top.x, tex.top.y+tex.top.h}, light, skyLight },
                { {x+1, y+h, z+1}, {0, 1, 0}, {tex.top.x+tex.top.w, tex.top.y+tex.top.h}, light, skyLight }
        );
        DebugStats::triCount += 1;
    }
    
    if(adj.front) {
        float light = getLight(cx, y, cz+1);
        float skyLight = getSkyLight(cx, y, cz+1);
        generateQuadMesh(mesh,
            { {x,   y+h, z+1}, {0, 0, -1}, {tex.front.x, tex.front.y+tex.front.h}, light, skyLight },
            { {x,   y,   z+1}, {0, 0, -1}, {tex.front.x, tex.front.y}, light, skyLight },
            { {x+1, y,   z+1}, {0, 0, -1}, {tex.front.x+tex.front.w, tex.front.y}, light, skyLight },
            { {x+1, y+h, z+1}, {0, 0, -1}, {tex.front.x+tex.front.w, tex.front.y+tex.front.h}, light, skyLight }
        );
        DebugStats::triCount += 1;
    }
    
    if(adj.right) {
        float light = getLight(cx+1, y, cz);
        float skyLight = getSkyLight(cx+1, y, cz);
        generateQuadMesh(mesh,
            { {x+1, y+h, z+1}, {-1, 0, 0}, {tex.left.x, tex.left.y+tex.left.h}, light, skyLight },
            { {x+1, y,   z+1}, {-1, 0, 0}, {tex.left.x, tex.left.y}, light, skyLight },
            { {x+1, y,   z},   {-1, 0, 0}, {tex.left.x+tex.left.w, tex.left.y}, light, skyLight },
            { {x+1, y+h, z},   {-1, 0, 0}, {tex.left.x+tex.left.w, tex.left.y+tex.left.h}, light, skyLight }
        );
        DebugStats::triCount += 1;
    }
    
    if(adj.back) {
        float light = getLight(cx, y, cz-1);
        float skyLight = getSkyLight(cx, y, cz-1);
        generateQuadMesh(mesh,
                { {x,   y+h, z},   {0, 0, 1}, {tex.back.x+tex.back.w, tex.back.y+tex.back.h}, light, skyLight },
                { {x+1, y+h, z},   {0, 0, 1}, {tex.back.x, tex.back.y+tex.back.h}, light, skyLight },
                { {x+1, y,   z},   {0, 0, 1}, {tex.back.x, tex.back.y}, light, skyLight },
                { {x,   y,   z},   {0, 0, 1}, {tex.back.x+tex.back.w, tex.back.y}, light, skyLight }
        );
        DebugStats::triCount += 1;
    }
    
    if(adj.left) {
        float light = getLight(cx-1, y, cz);
        float skyLight = getSkyLight(cx-1, y, cz);
        generateQuadMesh(mesh,
                { {x, y+h, z+1},   {1, 0, 0}, {tex.right.x+tex.right.w, tex.right.y+tex.right.h}, light, skyLight },
                { {x, y+h, z},     {1, 0, 0}, {tex.right.x, tex.right.y+tex.right.h}, light, skyLight },
                { {x, y,   z},     {1, 0, 0}, {tex.right.x, tex.right.y}, light, skyLight },
                { {x, y,   z+1},   {1, 0, 0}, {tex.right.x+tex.right.w, tex.right.y}, light, skyLight }
        );
        DebugStats::triCount += 1;
    }
    
    
    if(adj.bottom) {
        float light = getLight(cx, y-1, cz);
        float skyLight = getSkyLight(cx, y-1, cz);
        generateQuadMesh(mesh,
                { {x,   y, z+1}, {0, -1, 0}, {tex.bottom.x, tex.bottom.y+tex.bottom.h}, light, skyLight },
                { {x,   y, z},   {0, -1, 0}, {tex.bottom.x, tex.bottom.y}, light, skyLight },
                { {x+1, y, z},   {0, -1, 0}, {tex.bottom.x+tex.bottom.w, tex.bottom.y}, light, skyLight },
                { {x+1, y, z+1}, {0, -1, 0}, {tex.bottom.x+tex.bottom.w, tex.bottom.y+tex.bottom.h}, light, skyLight }
        );
        DebugStats::triCount += 1;
    }
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
    BlockID id;
    bool top, bottom, left, right, front, back;
    Block b;
    for(int y = 0; y < chunkH; y++)
        for(int x = 0; x < chunkW; x++)
            for(int z = 0; z < chunkL; z++) {
                light[y][x][z] = skyLight[y][x][z] = skyLightSpread[y][x][z] = 0;
                id = blocks[y][x][z];
                if(id == Blocks::airBlockID || id == Blocks::nullBlockID)
                    continue;
                b = Blocks::getBlockFromID(id);
                if(b.transparent) {
                    top = y == chunkH-1 || !blocks[y+1][x][z],
                    bottom = y != 0 && !blocks[y-1][x][z],
                    left = x == 0 ? chunks.left && !chunks.left->blocks[y][chunkW-1][z] : !blocks[y][x-1][z],
                    right = x == chunkW-1 ? chunks.right && !chunks.right->blocks[y][0][z] : !blocks[y][x+1][z],
                    front = z == chunkL-1 ? chunks.front && !chunks.front->blocks[y][x][0] : !blocks[y][x][z+1],
                    back = z == 0 ? chunks.back && !chunks.back->blocks[y][x][chunkL-1] : !blocks[y][x][z-1];
                } else {
                    top = y == chunkH-1 || Blocks::getBlockFromID(blocks[y+1][x][z]).transparent,
                    bottom = y != 0 && Blocks::getBlockFromID(blocks[y-1][x][z]).transparent,
                    left = x == 0 ? chunks.left && Blocks::getBlockFromID(chunks.left->blocks[y][chunkW-1][z]).transparent : Blocks::getBlockFromID(blocks[y][x-1][z]).transparent,
                    right = x == chunkW-1 ? chunks.right && Blocks::getBlockFromID(chunks.right->blocks[y][0][z]).transparent : Blocks::getBlockFromID(blocks[y][x+1][z]).transparent,
                    front = z == chunkL-1 ? chunks.front && Blocks::getBlockFromID(chunks.front->blocks[y][x][0]).transparent : Blocks::getBlockFromID(blocks[y][x][z+1]).transparent,
                    back = z == 0 ? chunks.back && Blocks::getBlockFromID(chunks.back->blocks[y][x][chunkL-1]).transparent : Blocks::getBlockFromID(blocks[y][x][z-1]).transparent;
                }
                if(top || bottom || left || right || front || back) {
                    if(y < minY) minY = y;
                    if(y > maxY) maxY = y;
                    
                    if(b.lightEmit > 0.f)
                        lights.push_back({{x, y, z}, b.lightEmit});
                }
            }
    for(int x = 0; x < chunkW; x++)
        for(int z = 0; z < chunkL; z++)
            calculateSkyLighting(x, maxY+1, z, 1.f);
    
    for(const auto &p : lights)
        calculateLighting(p.pos.x, p.pos.y, p.pos.z, p.val);
    
    float l, sl;
    for(int y = minY; y <= maxY; y++) {
        for(int x = 0; x < chunkW; x++) {
            for(int z = 0; z < chunkL; z++) {
                id = blocks[y][x][z];
                if(id == Blocks::airBlockID || id == Blocks::nullBlockID)
                    continue;
                
                b = Blocks::getBlockFromID(blocks[y][x][z]);
                // const float l = (float)(1+light[y][x][z])/16.f;
                l = (float)(.5f+light[y][x][z])/15.5f;
                sl = (float)(.5f+glm::max(skyLight[y][x][z], skyLightSpread[y][x][z]))/15.5f;
                // const float l = Blocks::getBlockFromID(id).lightBlocking;
                
                SurroundingBlocks adj;
                if(b.transparent) {
                    adj.top = y == chunkH-1 || !blocks[y+1][x][z];
                    adj.bottom = y != 0 && !blocks[y-1][x][z];
                    adj.left = x == 0 ? chunks.left && !chunks.left->blocks[y][chunkW-1][z] : !blocks[y][x-1][z];
                    adj.right = x == chunkW-1 ? chunks.right && !chunks.right->blocks[y][0][z] : !blocks[y][x+1][z];
                    adj.front = z == chunkL-1 ? chunks.front && !chunks.front->blocks[y][x][0] : !blocks[y][x][z+1];
                    adj.back = z == 0 ? chunks.back && !chunks.back->blocks[y][x][chunkL-1] : !blocks[y][x][z-1];
                    
                    if(top || bottom || left || right || front || back) {
                        if(b.liquid)
                            Renderer::generateLiquidMesh(transparentMesh, {pos.x + x, y, pos.y + z}, b.tex, adj, l, sl);
                        else
                            generateCubeMesh(transparentMesh, x, y, z, b.tex, chunks, adj);
                    }
                } else {
                    adj.top = y == chunkH-1 || Blocks::getBlockFromID(blocks[y+1][x][z]).transparent;
                    adj.bottom = y != 0 && Blocks::getBlockFromID(blocks[y-1][x][z]).transparent;
                    adj.left = x == 0 ? chunks.left && Blocks::getBlockFromID(chunks.left->blocks[y][chunkW-1][z]).transparent : Blocks::getBlockFromID(blocks[y][x-1][z]).transparent;
                    adj.right = x == chunkW-1 ? chunks.right && Blocks::getBlockFromID(chunks.right->blocks[y][0][z]).transparent : Blocks::getBlockFromID(blocks[y][x+1][z]).transparent;
                    adj.front = z == chunkL-1 ? chunks.front && Blocks::getBlockFromID(chunks.front->blocks[y][x][0]).transparent : Blocks::getBlockFromID(blocks[y][x][z+1]).transparent;
                    adj.back = z == 0 ? chunks.back && Blocks::getBlockFromID(chunks.back->blocks[y][x][chunkL-1]).transparent : Blocks::getBlockFromID(blocks[y][x][z-1]).transparent;
                    
                    if(top || bottom || left || right || front || back) {
                        // Renderer::generateCubeMesh(mesh, {pos.x + x, y, pos.y + z}, b.tex, adj, l, sl);
                        generateCubeMesh(mesh, x, y, z, b.tex, chunks, adj);
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

float Chunk::getLight(int x, int y, int z) {
    if(x < 0)
        x = 0;
    else if(x >= Chunk::chunkW)
        x = Chunk::chunkW-1;
    
    if(z < 0)
        z = 0;
    else if(z >= Chunk::chunkL)
        z = Chunk::chunkL-1;
    
    if(y < 0)
        y = 0;
    else if(y >= Chunk::chunkH)
        y = Chunk::chunkH-1;
    
    return (float)(.5f+light[y][x][z])/15.5f;
}

float Chunk::getSkyLight(int x, int y, int z) {
    if(x < 0)
        x = 0;
    else if(x >= Chunk::chunkW)
        x = Chunk::chunkW-1;
    
    if(z < 0)
        z = 0;
    else if(z >= Chunk::chunkL)
        z = Chunk::chunkL-1;
    
    if(y < 0)
        y = 0;
    else if(y >= Chunk::chunkH)
        y = Chunk::chunkH-1;
    
    return (float)(.5f+glm::max(skyLight[y][x][z], skyLightSpread[y][x][z]))/15.5f;
}
