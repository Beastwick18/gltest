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
                // skyLight[y][x][z] = 1;
                light[y][x][z] = 1;
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
    if(prev <= 0|| x < 0 || y < minY || y < 0 || z < 0 || x >= chunkW || y > maxY+1 || y >= chunkH || z >= chunkL)
        return;
    unsigned char currLight = light[y][x][z] >> 4;
    unsigned char newLight = (unsigned char)(prev*15);
    if(newLight <= currLight)
        return;
    
    float lb = .2f;
    setSkyLight(x, y, z, newLight);
    // skyLight[y][x][z] = glm::max(skyLight[y][x][z], (unsigned char)(prev*15));
    if(auto b = blocks[y][x][z]; b != Blocks::airBlockID && b != Blocks::nullBlockID) {
        prev = prev - Blocks::getBlockFromID(b).lightBlocking;
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
    if(prev <= 0 || x < 0 || y < minY || y < 0 || z < 0 || x >= chunkW || y > maxY+1 || y >= chunkH || z >= chunkL)
        return;
    float currLight = light[y][x][z] >> 4;
    float newLight = (unsigned char)(prev * 15);
    if(newLight <= currLight)
        return;
    float lb = .2f;
    setSkyLight(x, y, z, newLight);
    // skyLight[y][x][z] = glm::max(skyLight[y][x][z], (unsigned char)(prev*15));
    if(const auto &b = blocks[y][x][z]; !Blocks::getBlockFromID(b).transparent && b != Blocks::nullBlockID) {
        return;
    }
    calculateSkyLightingSpread(x, y+1, z, prev - lb);
    calculateSkyLightingSpread(x, y-1, z, prev - lb);
    calculateSkyLightingSpread(x-1, y, z, prev - lb);
    calculateSkyLightingSpread(x+1, y, z, prev - lb);
    calculateSkyLightingSpread(x, y, z-1, prev - lb);
    calculateSkyLightingSpread(x, y, z+1, prev - lb);
}

void Chunk::calculateLighting(int x, int y, int z, float prev) {
    float newLight = (unsigned char) (prev * 15);
    float currLight = getLight(x, y, z);
    if(newLight <= currLight || prev <= 0 || x < 0 || y < minY || y < 0 || z < 0 || x >= chunkW || y > maxY+1 || y >= chunkH || z >= chunkL)
        return;
    float lb = .15f;
    setLight(x, y, z, newLight);
    // light[y][x][z] = glm::max(light[y][x][z], (unsigned char)(prev*15));
    // if(auto b = blocks[y][x][z]; b != Blocks::airBlockID && b != Blocks::nullBlockID) {
    if(const auto &b = blocks[y][x][z]; !Blocks::getBlockFromID(b).transparent && b != Blocks::nullBlockID && prev != 1.f)
        return;
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

void Chunk::generateCubeMesh(Mesh<Vertex> &mesh, int cx, int y, int cz, BlockTexture tex, SurroundingBlocks adj) {
    float x = pos.x + cx;
    float z = pos.y + cz;
    if(adj.front) {
        unsigned char light = getLight(cx, y, cz+1);
        unsigned char skyLight = getSkyLight(cx, y, cz+1);
        unsigned char l = (skyLight << 4) | light;
        generateQuadMesh(mesh,
            { {x,   y+1, z+1}, {0, 0, -1}, {tex.front.x, tex.front.y+tex.front.h}, l},
            { {x,   y,   z+1}, {0, 0, -1}, {tex.front.x, tex.front.y}, l},
            { {x+1, y,   z+1}, {0, 0, -1}, {tex.front.x+tex.front.w, tex.front.y}, l},
            { {x+1, y+1, z+1}, {0, 0, -1}, {tex.front.x+tex.front.w, tex.front.y+tex.front.h}, l}
        );
        DebugStats::triCount += 1;
    }
    
    if(adj.right) {
        unsigned char light = getLight(cx+1, y, cz);
        unsigned char skyLight = getSkyLight(cx+1, y, cz);
        unsigned char l = (skyLight << 4) | light;
        generateQuadMesh(mesh,
            { {x+1, y+1, z+1}, {-1, 0, 0}, {tex.left.x, tex.left.y+tex.left.h}, l},
            { {x+1, y,   z+1}, {-1, 0, 0}, {tex.left.x, tex.left.y}, l},
            { {x+1, y,   z},   {-1, 0, 0}, {tex.left.x+tex.left.w, tex.left.y}, l},
            { {x+1, y+1, z},   {-1, 0, 0}, {tex.left.x+tex.left.w, tex.left.y+tex.left.h}, l}
        );
        DebugStats::triCount += 1;
    }
    
    if(adj.back) {
        unsigned char light = getLight(cx, y, cz-1);
        unsigned char skyLight = getSkyLight(cx, y, cz-1);
        unsigned char l = (skyLight << 4) | light;
        generateQuadMesh(mesh,
                { {x,   y+1, z},   {0, 0, 1}, {tex.back.x+tex.back.w, tex.back.y+tex.back.h}, l},
                { {x+1, y+1, z},   {0, 0, 1}, {tex.back.x, tex.back.y+tex.back.h}, l},
                { {x+1, y,   z},   {0, 0, 1}, {tex.back.x, tex.back.y}, l},
                { {x,   y,   z},   {0, 0, 1}, {tex.back.x+tex.back.w, tex.back.y}, l}
        );
        DebugStats::triCount += 1;
    }
    
    if(adj.left) {
        unsigned char light = getLight(cx-1, y, cz);
        unsigned char skyLight = getSkyLight(cx-1, y, cz);
        unsigned char l = (skyLight << 4) | light;
        generateQuadMesh(mesh,
                { {x, y+1, z+1},   {1, 0, 0}, {tex.right.x+tex.right.w, tex.right.y+tex.right.h}, l},
                { {x, y+1, z},     {1, 0, 0}, {tex.right.x, tex.right.y+tex.right.h}, l},
                { {x, y,   z},     {1, 0, 0}, {tex.right.x, tex.right.y}, l},
                { {x, y,   z+1},   {1, 0, 0}, {tex.right.x+tex.right.w, tex.right.y}, l}
        );
        DebugStats::triCount += 1;
    }
    
    if(adj.top) {
        unsigned char light = getLight(cx, y+1, cz);
        unsigned char skyLight = getSkyLight(cx, y+1, cz);
        unsigned char l = (skyLight << 4) | light;
        generateQuadMesh(mesh,
                { {x+1, y+1, z},   {0, 1, 0}, {tex.top.x+tex.top.w, tex.top.y}, l},
                { {x,   y+1, z},   {0, 1, 0}, {tex.top.x, tex.top.y}, l},
                { {x,   y+1, z+1}, {0, 1, 0}, {tex.top.x, tex.top.y+tex.top.h}, l},
                { {x+1, y+1, z+1}, {0, 1, 0}, {tex.top.x+tex.top.w, tex.top.y+tex.top.h}, l}
        );
        DebugStats::triCount += 1;
    }
    
    if(adj.bottom) {
        unsigned char light = getLight(cx, y-1, cz);
        unsigned char skyLight = getSkyLight(cx, y-1, cz);
        unsigned char l = (skyLight << 4) | light;
        generateQuadMesh(mesh,
                { {x,   y, z+1}, {0, -1, 0}, {tex.bottom.x, tex.bottom.y+tex.bottom.h}, l},
                { {x,   y, z},   {0, -1, 0}, {tex.bottom.x, tex.bottom.y}, l},
                { {x+1, y, z},   {0, -1, 0}, {tex.bottom.x+tex.bottom.w, tex.bottom.y}, l},
                { {x+1, y, z+1}, {0, -1, 0}, {tex.bottom.x+tex.bottom.w, tex.bottom.y+tex.bottom.h}, l}
        );
        DebugStats::triCount += 1;
    }
    
}

void Chunk::generateLiquidMesh(Mesh<Vertex> &mesh, int cx, int y, int cz, BlockTexture tex, SurroundingBlocks adj) {
    float x = pos.x + cx;
    float z = pos.y + cz;
    float h = 1.f;
    if(adj.top) {
        // float light = getLight(cx, y+1, cz);
        // float skyLight = getSkyLight(cx, y+1, cz);
        tex.left.h *= 15.f/16.f;
        tex.right.h *= 15.f/16.f;
        tex.front.h *= 15.f/16.f;
        tex.back.h *= 15.f/16.f;
        h = 15.f/16.f;
        generateQuadMesh(mesh,
                { {x+1, y+h, z},   {0, 1, 0}, {tex.top.x+tex.top.w, tex.top.y}, light[y+1][cx][cz] },
                { {x,   y+h, z},   {0, 1, 0}, {tex.top.x, tex.top.y}, light[y+1][cx][cz] },
                { {x,   y+h, z+1}, {0, 1, 0}, {tex.top.x, tex.top.y+tex.top.h}, light[y+1][cx][cz] },
                { {x+1, y+h, z+1}, {0, 1, 0}, {tex.top.x+tex.top.w, tex.top.y+tex.top.h}, light[y+1][cx][cz] }
        );
        DebugStats::triCount += 1;
    }
    
    if(adj.front) {
        unsigned char light = getLight(cx, y, cz+1);
        unsigned char skyLight = getSkyLight(cx, y, cz+1);
        unsigned char l = (skyLight << 4) | light;
        generateQuadMesh(mesh,
            { {x,   y+h, z+1}, {0, 0, -1}, {tex.front.x, tex.front.y+tex.front.h}, l},
            { {x,   y,   z+1}, {0, 0, -1}, {tex.front.x, tex.front.y}, l},
            { {x+1, y,   z+1}, {0, 0, -1}, {tex.front.x+tex.front.w, tex.front.y}, l},
            { {x+1, y+h, z+1}, {0, 0, -1}, {tex.front.x+tex.front.w, tex.front.y+tex.front.h}, l}
        );
        DebugStats::triCount += 1;
    }
    
    if(adj.right) {
        unsigned char light = getLight(cx+1, y, cz);
        unsigned char skyLight = getSkyLight(cx+1, y, cz);
        unsigned char l = (skyLight << 4) | light;
        generateQuadMesh(mesh,
            { {x+1, y+h, z+1}, {-1, 0, 0}, {tex.left.x, tex.left.y+tex.left.h}, l},
            { {x+1, y,   z+1}, {-1, 0, 0}, {tex.left.x, tex.left.y}, l},
            { {x+1, y,   z},   {-1, 0, 0}, {tex.left.x+tex.left.w, tex.left.y}, l},
            { {x+1, y+h, z},   {-1, 0, 0}, {tex.left.x+tex.left.w, tex.left.y+tex.left.h}, l}
        );
        DebugStats::triCount += 1;
    }
    
    if(adj.back) {
        unsigned char light = getLight(cx, y, cz-1);
        unsigned char skyLight = getSkyLight(cx, y, cz-1);
        unsigned char l = (skyLight << 4) | light;
        generateQuadMesh(mesh,
                { {x,   y+h, z},   {0, 0, 1}, {tex.back.x+tex.back.w, tex.back.y+tex.back.h}, l},
                { {x+1, y+h, z},   {0, 0, 1}, {tex.back.x, tex.back.y+tex.back.h}, l},
                { {x+1, y,   z},   {0, 0, 1}, {tex.back.x, tex.back.y}, l},
                { {x,   y,   z},   {0, 0, 1}, {tex.back.x+tex.back.w, tex.back.y}, l}
        );
        DebugStats::triCount += 1;
    }
    
    if(adj.left) {
        unsigned char light = getLight(cx-1, y, cz);
        unsigned char skyLight = getSkyLight(cx-1, y, cz);
        unsigned char l = (skyLight << 4) | light;
        generateQuadMesh(mesh,
                { {x, y+h, z+1},   {1, 0, 0}, {tex.right.x+tex.right.w, tex.right.y+tex.right.h}, l},
                { {x, y+h, z},     {1, 0, 0}, {tex.right.x, tex.right.y+tex.right.h}, l},
                { {x, y,   z},     {1, 0, 0}, {tex.right.x, tex.right.y}, l},
                { {x, y,   z+1},   {1, 0, 0}, {tex.right.x+tex.right.w, tex.right.y}, l}
        );
        DebugStats::triCount += 1;
    }
    
    
    if(adj.bottom) {
        unsigned char light = getLight(cx, y-1, cz);
        unsigned char skyLight = getSkyLight(cx, y-1, cz);
        unsigned char l = (skyLight << 4) | light;
        generateQuadMesh(mesh,
                { {x,   y, z+1}, {0, -1, 0}, {tex.bottom.x, tex.bottom.y+tex.bottom.h}, l},
                { {x,   y, z},   {0, -1, 0}, {tex.bottom.x, tex.bottom.y}, l},
                { {x+1, y, z},   {0, -1, 0}, {tex.bottom.x+tex.bottom.w, tex.bottom.y}, l},
                { {x+1, y, z+1}, {0, -1, 0}, {tex.bottom.x+tex.bottom.w, tex.bottom.y+tex.bottom.h}, l}
        );
        DebugStats::triCount += 1;
    }
}

void Chunk::generateTorchMesh(Mesh<Vertex> &mesh, int cx, int y, int cz, BlockTexture tex, SurroundingBlocks adj) {
    float x = pos.x + cx;
    float z = pos.y + cz;
    unsigned char light = getLight(cx, y, cz+1);
    unsigned char skyLight = getSkyLight(cx, y, cz+1);
    unsigned char l = (skyLight << 4) | light;
    // generateQuadMesh(mesh,
    //         { {x+9.f/16.f, y+10.f/16.f, z+7.f/16.f},   {0, 1, 0}, {tex.top.x+tex.top.w * 9.f/16.f, tex.top.y + tex.top.w * 8.f/16.f}, light, skyLight },
    //         { {x+7.f/16.f,   y+10.f/16.f, z+7.f/16.f},   {0, 1, 0}, {tex.top.x + tex.top.w * 7.f/16.f, tex.top.y + tex.top.w * 8.f/16.f}, light, skyLight },
    //         { {x+7.f/16.f,   y+10.f/16.f, z+9.f/16.f}, {0, 1, 0}, {tex.top.x + tex.top.w * 7.f/16.f, tex.top.y+tex.top.h * 10.f/16.f}, light, skyLight },
    //         { {x+9.f/16.f, y+10.f/16.f, z+9.f/16.f}, {0, 1, 0}, {tex.top.x+tex.top.w * 9.f/16.f, tex.top.y+tex.top.h * 10.f/16.f}, light, skyLight }
    // );
    generateQuadMesh(mesh,
        { {x,   y+1, z+9.f/16.f}, {0, 0, -1}, {tex.front.x, tex.front.y+tex.front.h}, l},
        { {x,   y,   z+9.f/16.f}, {0, 0, -1}, {tex.front.x, tex.front.y}, l},
        { {x+1, y,   z+9.f/16.f}, {0, 0, -1}, {tex.front.x+tex.front.w, tex.front.y}, l},
        { {x+1, y+1, z+9.f/16.f}, {0, 0, -1}, {tex.front.x+tex.front.w, tex.front.y+tex.front.h}, l}
    );
    DebugStats::triCount += 1;

    // light = getLight(cx+1, y, cz);
    // skyLight = getSkyLight(cx+1, y, cz);
    generateQuadMesh(mesh,
        { {x+9.f/16.f, y+1, z+1}, {-1, 0, 0}, {tex.left.x, tex.left.y+tex.left.h}, l},
        { {x+9.f/16.f, y,   z+1}, {-1, 0, 0}, {tex.left.x, tex.left.y}, l},
        { {x+9.f/16.f, y,   z},   {-1, 0, 0}, {tex.left.x+tex.left.w, tex.left.y}, l},
        { {x+9.f/16.f, y+1, z},   {-1, 0, 0}, {tex.left.x+tex.left.w, tex.left.y+tex.left.h}, l}
    );
    DebugStats::triCount += 1;

    // light = getLight(cx, y, cz-1);
    // skyLight = getSkyLight(cx, y, cz-1);
    generateQuadMesh(mesh,
            { {x,   y+1, z+7.f/16.f},   {0, 0, 1}, {tex.back.x+tex.back.w, tex.back.y+tex.back.h}, l},
            { {x+1, y+1, z+7.f/16.f},   {0, 0, 1}, {tex.back.x, tex.back.y+tex.back.h}, l},
            { {x+1, y,   z+7.f/16.f},   {0, 0, 1}, {tex.back.x, tex.back.y}, l},
            { {x,   y,   z+7.f/16.f},   {0, 0, 1}, {tex.back.x+tex.back.w, tex.back.y}, l}
    );
    DebugStats::triCount += 1;

    // light = getLight(cx-1, y, cz);
    // skyLight = getSkyLight(cx-1, y, cz);
    generateQuadMesh(mesh,
            { {x+7.f/16.f, y+1, z+1},   {1, 0, 0}, {tex.right.x+tex.right.w, tex.right.y+tex.right.h}, l},
            { {x+7.f/16.f, y+1, z},     {1, 0, 0}, {tex.right.x, tex.right.y+tex.right.h}, l},
            { {x+7.f/16.f, y,   z},     {1, 0, 0}, {tex.right.x, tex.right.y}, l},
            { {x+7.f/16.f, y,   z+1},   {1, 0, 0}, {tex.right.x+tex.right.w, tex.right.y}, l}
    );
    DebugStats::triCount += 1;

    // light = getLight(cx, y+1, cz);
    // skyLight = getSkyLight(cx, y+1, cz);
    generateQuadMesh(mesh,
            { {x+9.f/16.f, y+10.f/16.f, z+7.f/16.f},   {0, 1, 0}, {tex.top.x+tex.top.w * 9.f/16.f, tex.top.y + tex.top.h * 8.f/16.f}, l},
            { {x+7.f/16.f,   y+10.f/16.f, z+7.f/16.f},   {0, 1, 0}, {tex.top.x + tex.top.w * 7.f/16.f, tex.top.y + tex.top.h * 8.f/16.f}, l},
            { {x+7.f/16.f,   y+10.f/16.f, z+9.f/16.f}, {0, 1, 0}, {tex.top.x + tex.top.w * 7.f/16.f, tex.top.y+tex.top.h * 10.f/16.f}, l},
            { {x+9.f/16.f, y+10.f/16.f, z+9.f/16.f}, {0, 1, 0}, {tex.top.x+tex.top.w * 9.f/16.f, tex.top.y+tex.top.h * 10.f/16.f}, l}
    );
    DebugStats::triCount += 1;

// // No bottom on torches
// if(adj.bottom) {
//     float light = getLight(cx, y-1, cz);
//     float skyLight = getSkyLight(cx, y-1, cz);
//     generateQuadMesh(mesh,
//             { {x,   y, z+1}, {0, -1, 0}, {tex.bottom.x, tex.bottom.y+tex.bottom.h}, light, skyLight },
//             { {x,   y, z},   {0, -1, 0}, {tex.bottom.x, tex.bottom.y}, light, skyLight },
//             { {x+1, y, z},   {0, -1, 0}, {tex.bottom.x+tex.bottom.w, tex.bottom.y}, light, skyLight },
//             { {x+1, y, z+1}, {0, -1, 0}, {tex.bottom.x+tex.bottom.w, tex.bottom.y+tex.bottom.h}, light, skyLight }
//     );
//     DebugStats::triCount += 1;
// }
}

bool transparentVisible(BlockID id, BlockID other) {
    return other != id && Blocks::getBlockFromID(other).transparent;
}

bool opaqueVisible(BlockID other) {
    return Blocks::getBlockFromID(other).transparent;
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
    bool visible;
    Block b;
    for(int y = 0; y < chunkH; y++)
        for(int x = 0; x < chunkW; x++)
            for(int z = 0; z < chunkL; z++) {
                // light[y][x][z] = skyLight[y][x][z] = 0;
                light[y][x][z] = 0;
                id = blocks[y][x][z];
                if(id == Blocks::airBlockID || id == Blocks::nullBlockID)
                    continue;
                b = Blocks::getBlockFromID(id);
                visible = false;
                if( b.transparent && ( ( y == chunkH-1 || transparentVisible(id, blocks[y+1][x][z]) )
                    || ( y != 0 && transparentVisible(id, blocks[y-1][x][z]) )
                    || ( x == 0
                        ? chunks.left && transparentVisible(id, chunks.left->blocks[y][chunkW-1][z])
                        : transparentVisible(id, blocks[y][x-1][z]) )
                    || ( x == chunkW-1
                        ? chunks.right && transparentVisible(id, chunks.right->blocks[y][0][z])
                        : transparentVisible(id, blocks[y][x+1][z]) )
                    || ( z == chunkL-1
                        ? chunks.front && transparentVisible(id, chunks.front->blocks[y][x][0])
                        : transparentVisible(id, blocks[y][x][z+1]) )
                    || ( z == 0
                        ? chunks.back && transparentVisible(id, chunks.back->blocks[y][x][chunkL-1])
                        : transparentVisible(id, blocks[y][x][z-1]) ) ) ) {
                    visible = true;
                } else if(( y == chunkH-1 || opaqueVisible(blocks[y+1][x][z]) )
                    || ( y != 0 && opaqueVisible(blocks[y-1][x][z]) )
                    || ( x == 0
                        ? chunks.left && opaqueVisible(chunks.left->blocks[y][chunkW-1][z])
                        : opaqueVisible(blocks[y][x-1][z]) )
                    || ( x == chunkW-1
                        ? chunks.right && opaqueVisible(chunks.right->blocks[y][0][z])
                        : opaqueVisible(blocks[y][x+1][z]) )
                    || ( z == chunkL-1
                        ? chunks.front && opaqueVisible(chunks.front->blocks[y][x][0])
                        : opaqueVisible(blocks[y][x][z+1]) )
                    || ( z == 0
                        ? chunks.back && opaqueVisible(chunks.back->blocks[y][x][chunkL-1])
                        : opaqueVisible(blocks[y][x][z-1]) ) ) {
                    visible = true;
                }
                if(visible) {
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
    
    for(int y = minY; y <= maxY; y++) {
        for(int x = 0; x < chunkW; x++) {
            for(int z = 0; z < chunkL; z++) {
                id = blocks[y][x][z];
                if(id == Blocks::airBlockID || id == Blocks::nullBlockID)
                    continue;
                
                b = Blocks::getBlockFromID(blocks[y][x][z]);
                
                SurroundingBlocks adj;
                if(b.id == 13 || b.id == 14)
                    generateTorchMesh(transparentMesh, x, y, z, b.tex, adj);
                else if(b.transparent) {
                    adj.top = ( y == chunkH-1 ) || transparentVisible(id, blocks[y+1][x][z]);
                    adj.bottom = ( y != 0 ) && transparentVisible(id, blocks[y-1][x][z]);
                    adj.left = ( x == 0 )
                        ? chunks.left && transparentVisible(id, chunks.left->blocks[y][chunkW-1][z])
                        : transparentVisible(id, blocks[y][x-1][z]);
                    adj.right = ( x == chunkW-1 )
                        ? chunks.right && transparentVisible(id, chunks.right->blocks[y][0][z])
                        : transparentVisible(id, blocks[y][x+1][z]);
                    adj.front = ( z == chunkL-1 )
                        ? chunks.front && transparentVisible(id, chunks.front->blocks[y][x][0])
                        : transparentVisible(id, blocks[y][x][z+1]);
                    adj.back = ( z == 0 )
                        ? chunks.back && transparentVisible(id, chunks.back->blocks[y][x][chunkL-1])
                        : transparentVisible(id, blocks[y][x][z-1]);
                    
                    if(adj.top || adj.bottom || adj.left || adj.right || adj.front || adj.back) {
                        if(b.liquid)
                            generateLiquidMesh(transparentMesh, x, y, z, b.tex, adj);
                        else
                            generateCubeMesh(transparentMesh, x, y, z, b.tex, adj);
                    }
                } else {
                    adj.top = y == chunkH-1 || opaqueVisible(blocks[y+1][x][z]);
                    adj.bottom = y != 0 && opaqueVisible(blocks[y-1][x][z]);
                    adj.left = x == 0
                        ? chunks.left && opaqueVisible(chunks.left->blocks[y][chunkW-1][z])
                        : opaqueVisible(blocks[y][x-1][z]);
                    adj.right = x == chunkW-1
                        ? chunks.right && opaqueVisible(chunks.right->blocks[y][0][z])
                        : opaqueVisible(blocks[y][x+1][z]);
                    adj.front = z == chunkL-1
                        ? chunks.front && opaqueVisible(chunks.front->blocks[y][x][0])
                        : opaqueVisible(blocks[y][x][z+1]);
                    adj.back = z == 0
                        ? chunks.back && opaqueVisible(chunks.back->blocks[y][x][chunkL-1])
                        : opaqueVisible(blocks[y][x][z-1]);
                    
                    if(adj.top || adj.bottom || adj.left || adj.right || adj.front || adj.back)
                        generateCubeMesh(mesh, x, y, z, b.tex, adj);
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

unsigned char Chunk::getLight(int x, int y, int z) {
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
    
    return light[y][x][z] & 0xF;
}

unsigned char Chunk::getSkyLight(int x, int y, int z) {
    if(y == maxY+1) return 15;
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
    
    return light[y][x][z] >> 4;
}

void Chunk::setLight(int x, int y, int z, unsigned char value) {
    light[y][x][z] &= 0xF0;
    light[y][x][z] |= value & 0xF;
}

void Chunk::setSkyLight(int x, int y, int z, unsigned char value) {
    light[y][x][z] &= 0xF;
    light[y][x][z] |= value << 4;
}
