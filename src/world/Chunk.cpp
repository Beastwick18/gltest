#include <mutex>
#include "world/Chunk.h"
#include "glm/gtc/noise.hpp"
#include "renderer/texture2D.h"
#include "renderer/renderer.h"
#include "utils/DebugStats.h"
#include "world/World.h"
#include "glm/gtc/random.hpp"
#include <cstring>

Chunk::Chunk(int xx, int yy) : pos(xx*chunkW,yy*chunkL) {
    maxY = 0;
    minY = Chunk::chunkH;
    for(int y = 0; y < chunkH; y++)
        for(int x = 0; x < chunkW; x++)
            for(int z = 0; z < chunkL; z++) {
                blocks[y][x][z].id = 0;
                // skyLight[y][x][z] = 1;
                light[y][x][z] = 1;
            }
    status = EMPTY;
    mesh.init(chunkW * chunkH * chunkL / 2);
    waterMesh.init(chunkW * chunkH * chunkL / 2);
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
        return Blocks::NULL_BLOCK;
    
    return blocks[y][x][z].id;
}

void Chunk::addBlock(BlockID id, int x, int y, int z) {
    if(x < 0 || x >= chunkW || y < 0 || y >= chunkH || z < 0 || z >= chunkL)
        return;
    if(blocks[y][x][z].id && !Blocks::getBlockFromID(blocks[y][x][z].id).liquid)
        return;
    blocks[y][x][z].id = id;
}

void Chunk::removeBlock(int x, int y, int z) {
    if(x < 0 || x >= chunkW || y < 0 || y >= chunkH || z < 0 || z >= chunkL)
        return;
    blocks[y][x][z].id = Blocks::AIR_BLOCK;
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
    int height, stoneHeight;
    bool tree;
    status = BUILDING;
    for(int x = 0; x < chunkW; x++) {
        for(int z = 0; z < chunkL; z++) {
            height = getNoise(pos.x + x, pos.y + z)+70;
            stoneHeight = (float)height / 1.15f;
            tree = glm::linearRand(0, 80) == 1;
            
            blocks[0][x][z].id = Blocks::BEDROCK;
            for(int y = 1; y < height; y++) {
                if(y < stoneHeight-1)
                    blocks[y][x][z].id = Blocks::STONE;
                else if(y < height-1)
                    blocks[y][x][z].id = Blocks::DIRT;
                else if(y < 81) {
                    blocks[y][x][z].id = Blocks::SAND;
                    if(y < 80)
                        for(int w = y+1; w < 80; w++)
                            blocks[w][x][z].id = Blocks::WATER;
                } else
                    blocks[y][x][z].id = Blocks::GRASS;
            }
            if(height-1 >= 81) {
                if(tree && x > 1 && x < chunkW-2 && z > 1 && z < chunkL-2) {
                    int treeHeight = glm::linearRand(4, 7);
                    if(blocks[height-1][x][z].id == Blocks::GRASS)
                        blocks[height-1][x][z].id = Blocks::DIRT;
                    for(int y = height; y < height+treeHeight; y++)
                        blocks[y][x][z].id = Blocks::LOG;
                    
                    addBlock(Blocks::LEAVES, x, height + treeHeight, z);
                    
                    addBlock(Blocks::LEAVES, x - 1, height + treeHeight, z);
                    addBlock(Blocks::LEAVES, x + 1, height + treeHeight, z);
                    
                    addBlock(Blocks::LEAVES, x, height + treeHeight, z - 1);
                    addBlock(Blocks::LEAVES, x, height + treeHeight, z + 1);
                    
                    addBlock(Blocks::LEAVES, x - 1, height + treeHeight - 1, z);
                    addBlock(Blocks::LEAVES, x + 1, height + treeHeight - 1, z);
                    
                    addBlock(Blocks::LEAVES, x - 1, height + treeHeight - 1, z + 1);
                    addBlock(Blocks::LEAVES, x, height + treeHeight - 1, z + 1);
                    addBlock(Blocks::LEAVES, x + 1, height + treeHeight - 1, z + 1);
                    
                    addBlock(Blocks::LEAVES, x - 1, height + treeHeight - 1, z - 1);
                    addBlock(Blocks::LEAVES, x, height + treeHeight - 1, z - 1);
                    addBlock(Blocks::LEAVES, x + 1, height + treeHeight - 1, z - 1);
                    
                    for(int yy=height+treeHeight-3; yy < height+treeHeight-1; yy++)
                        for(int xx=x-2; xx < x+3; xx++)
                            for(int zz = z-2; zz < z+3; zz++)
                                if(xx != x || zz != z)
                                    blocks[yy][xx][zz].id = Blocks::LEAVES;
                }
                if(blocks[height][x][z].id == Blocks::AIR_BLOCK && blocks[height-1][x][z].id == Blocks::GRASS && glm::linearRand(0, 3) == 2) {
                    if(glm::linearRand(0, 4) == 0) {
                        BlockID b;
                        switch(glm::linearRand(0, 3)) {
                            case 0:
                                b = Blocks::ROSE;
                                break;
                            case 1:
                                b = Blocks::DANDELION;
                                break;
                            case 2:
                                b = Blocks::WHITE_TULIP;
                                break;
                            case 3:
                                b = Blocks::RED_MUSHROOM + glm::linearRand(0, 1);
                        }
                        blocks[height][x][z].id = b;
                    } else if(glm::linearRand(0, 10) == 1 && blocks[height+1][x][z].id == Blocks::AIR_BLOCK){
                        blocks[height][x][z].id = Blocks::VERY_TALL_GRASS;
                        blocks[height+1][x][z].id = Blocks::VERY_TALL_GRASS_TOP;
                    } else 
                        blocks[height][x][z].id = Blocks::TALL_GRASS;
                }
            } else if(height == 80 && tree) {
                blocks[height+0][x][z].id = Blocks::SUGAR_CANE;
                blocks[height+1][x][z].id = Blocks::SUGAR_CANE;
                blocks[height+2][x][z].id = Blocks::SUGAR_CANE;
            }
        }
    }
    status = BUILT;
}

void Chunk::calculateSkyLighting(int x, int y, int z, LightData prev) {
    if(prev <= (light[y][x][z] >> 4) || prev > 15 || x < 0 || y < minY || y < 0 || z < 0 || x >= chunkW || y > maxY || y >= chunkH || z >= chunkL)
        return;
    
    setSkyLight(x, y, z, prev);
    
    if(auto b = blocks[y][x][z].id; b != Blocks::AIR_BLOCK)
        prev = prev - Blocks::getBlockFromID(b).lightBlocking;
    
    LightData lb = 1;
    LightData hlb = (y != maxY) * lb;
    LightData vlb = (prev != 15) * lb;
    calculateSkyLighting(x, y-1, z, prev - vlb);
    calculateSkyLighting(x-1, y, z, prev - hlb);
    calculateSkyLighting(x+1, y, z, prev - hlb);
    calculateSkyLighting(x, y, z-1, prev - hlb);
    calculateSkyLighting(x, y, z+1, prev - hlb);
}

void Chunk::calculateLighting(int x, int y, int z, LightData prev) {
    if(prev <= getLight(x, y, z) || prev > 15 || x < 0 || y < minY || y < 0 || z < 0 || x >= chunkW || y > maxY+1 || y >= chunkH || z >= chunkL)
        return;
    
    setLight(x, y, z, prev);
    
    if(const auto &b = blocks[y][x][z].id; !Blocks::getBlockFromID(b).transparent && b != Blocks::NULL_BLOCK && prev != 15)
        return;
    
    LightData lb = 1;
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

void Chunk::generateCubeMesh(Mesh<Vertex> &mesh, int cx, int y, int cz, BlockTexture tex, unsigned char adj) {
    float x = pos.x + cx;
    float z = pos.y + cz;
    if(adj & 16) {
        // LightData light = getLight(cx, y, cz+1);
        // LightData skyLight = getSkyLight(cx, y, cz+1);
        // LightData lightData = (skyLight << 4) | light;
        unsigned int data = getLightData(cx, y, cz+1);
        data |= BlockOrientation::SOUTH << 8;
        generateQuadMesh(mesh,
            { {x,   y+1, z+1}, {tex.front.x, tex.front.y+tex.front.h}, data},
            { {x,   y,   z+1}, {tex.front.x, tex.front.y}, data},
            { {x+1, y,   z+1}, {tex.front.x+tex.front.w, tex.front.y}, data},
            { {x+1, y+1, z+1}, {tex.front.x+tex.front.w, tex.front.y+tex.front.h}, data}
        );
        DebugStats::triCount += 1;
    }
    
    if(adj & 8) {
        // LightData light = getLight(cx+1, y, cz);
        // LightData skyLight = getSkyLight(cx+1, y, cz);
        // LightData lightData = (skyLight << 4) | light;
        unsigned int data = getLightData(cx+1, y, cz);
        data |= BlockOrientation::EAST << 8;
        generateQuadMesh(mesh,
            { {x+1, y+1, z+1}, {tex.left.x, tex.left.y+tex.left.h}, data},
            { {x+1, y,   z+1}, {tex.left.x, tex.left.y}, data},
            { {x+1, y,   z},   {tex.left.x+tex.left.w, tex.left.y}, data},
            { {x+1, y+1, z},   {tex.left.x+tex.left.w, tex.left.y+tex.left.h}, data}
        );
        DebugStats::triCount += 1;
    }
    
    if(adj & 32) {
        // LightData light = getLight(cx, y, cz-1);
        // LightData skyLight = getSkyLight(cx, y, cz-1);
        // LightData lightData = (skyLight << 4) | light;
        unsigned int data = getLightData(cx, y, cz-1);
        data |= BlockOrientation::NORTH << 8;
        generateQuadMesh(mesh,
                { {x,   y+1, z}, {tex.back.x+tex.back.w, tex.back.y+tex.back.h}, data},
                { {x+1, y+1, z}, {tex.back.x, tex.back.y+tex.back.h}, data},
                { {x+1, y,   z}, {tex.back.x, tex.back.y}, data},
                { {x,   y,   z}, {tex.back.x+tex.back.w, tex.back.y}, data}
        );
        DebugStats::triCount += 1;
    }
    
    if(adj & 4) {
        // LightData light = getLight(cx-1, y, cz);
        // LightData skyLight = getSkyLight(cx-1, y, cz);
        // LightData lightData = (skyLight << 4) | light;
        unsigned int data = getLightData(cx-1, y, cz);
        data |= BlockOrientation::WEST << 8;
        generateQuadMesh(mesh,
                { {x, y+1, z+1}, {tex.right.x+tex.right.w, tex.right.y+tex.right.h}, data},
                { {x, y+1, z},   {tex.right.x, tex.right.y+tex.right.h}, data},
                { {x, y,   z},   {tex.right.x, tex.right.y}, data},
                { {x, y,   z+1}, {tex.right.x+tex.right.w, tex.right.y}, data}
        );
        DebugStats::triCount += 1;
    }
    
    if(adj & 1) {
        // LightData light = getLight(cx, y+1, cz);
        // LightData skyLight = getSkyLight(cx, y+1, cz);
        // LightData lightData = (skyLight << 4) | light;
        unsigned int data = getLightData(cx, y+1, cz);
        data |= BlockOrientation::UP << 8;
        generateQuadMesh(mesh,
                { {x+1, y+1, z},   {tex.top.x+tex.top.w, tex.top.y}, data},
                { {x,   y+1, z},   {tex.top.x, tex.top.y}, data},
                { {x,   y+1, z+1}, {tex.top.x, tex.top.y+tex.top.h}, data},
                { {x+1, y+1, z+1}, {tex.top.x+tex.top.w, tex.top.y+tex.top.h}, data}
        );
        DebugStats::triCount += 1;
    }
    
    if(adj & 2) {
        // LightData light = getLight(cx, y-1, cz);
        // LightData skyLight = getSkyLight(cx, y-1, cz);
        // LightData lightData = (skyLight << 4) | light;
        unsigned int data = getLightData(cx, y-1, cz);
        data |= BlockOrientation::DOWN << 8;
        generateQuadMesh(mesh,
                { {x,   y, z+1}, {tex.bottom.x, tex.bottom.y+tex.bottom.h}, data},
                { {x,   y, z},   {tex.bottom.x, tex.bottom.y}, data},
                { {x+1, y, z},   {tex.bottom.x+tex.bottom.w, tex.bottom.y}, data},
                { {x+1, y, z+1}, {tex.bottom.x+tex.bottom.w, tex.bottom.y+tex.bottom.h}, data}
        );
        DebugStats::triCount += 1;
    }
    
}

void Chunk::generateLiquidMesh(Mesh<Vertex> &mesh, int cx, int y, int cz, BlockTexture tex, unsigned char adj) {
    float x = pos.x + cx;
    float z = pos.y + cz;
    float h = 1.f;
    if(adj & 1) {
        tex.left.h *= 15.f/16.f;
        tex.right.h *= 15.f/16.f;
        tex.front.h *= 15.f/16.f;
        tex.back.h *= 15.f/16.f;
        h = 15.f/16.f;
        // LightData light = getLight(cx, y+1, cz);
        // LightData skyLight = getSkyLight(cx, y+1, cz);
        // unsigned int data = (skyLight << 4) | light;
        unsigned int data = getLightData(cx, y+1, cz);
        data |= BlockOrientation::UP << 8;
        generateQuadMesh(mesh,
                { {x+1, y+h, z},   {tex.top.x+tex.top.w, tex.top.y},  data},
                { {x,   y+h, z},   {tex.top.x, tex.top.y}, data },
                { {x,   y+h, z+1}, {tex.top.x, tex.top.y+tex.top.h}, data },
                { {x+1, y+h, z+1}, {tex.top.x+tex.top.w, tex.top.y+tex.top.h}, data }
        );
        DebugStats::triCount += 1;
    }
    
    if(adj & 16) {
        // LightData light = getLight(cx, y, cz+1);
        // LightData skyLight = getSkyLight(cx, y, cz+1);
        // unsigned int data = (skyLight << 4) | light;
        unsigned int data = getLightData(cx, y, cz+1);
        data |= BlockOrientation::SOUTH << 8;
        generateQuadMesh(mesh,
            { {x,   y+h, z+1},  {tex.front.x, tex.front.y+tex.front.h}, data},
            { {x,   y,   z+1},  {tex.front.x, tex.front.y}, data},
            { {x+1, y,   z+1},  {tex.front.x+tex.front.w, tex.front.y}, data},
            { {x+1, y+h, z+1},  {tex.front.x+tex.front.w, tex.front.y+tex.front.h}, data}
        );
        DebugStats::triCount += 1;
    }
    
    if(adj & 8) {
        // LightData light = getLight(cx+1, y, cz);
        // LightData skyLight = getSkyLight(cx+1, y, cz);
        // unsigned int data = (skyLight << 4) | light;
        unsigned int data = getLightData(cx+1, y, cz);
        data |= BlockOrientation::EAST << 8;
        generateQuadMesh(mesh,
            { {x+1, y+h, z+1},  {tex.left.x, tex.left.y+tex.left.h}, data},
            { {x+1, y,   z+1},  {tex.left.x, tex.left.y}, data},
            { {x+1, y,   z},    {tex.left.x+tex.left.w, tex.left.y}, data},
            { {x+1, y+h, z},    {tex.left.x+tex.left.w, tex.left.y+tex.left.h}, data}
        );
        DebugStats::triCount += 1;
    }
    
    if(adj & 32) {
        // LightData light = getLight(cx, y, cz-1);
        // LightData skyLight = getSkyLight(cx, y, cz-1);
        // unsigned int data = (skyLight << 4) | light;
        unsigned int data = getLightData(cx, y, cz-1);
        data |= BlockOrientation::NORTH << 8;
        generateQuadMesh(mesh,
                { {x,   y+h, z},   {tex.back.x+tex.back.w, tex.back.y+tex.back.h}, data},
                { {x+1, y+h, z},   {tex.back.x, tex.back.y+tex.back.h}, data},
                { {x+1, y,   z},   {tex.back.x, tex.back.y}, data},
                { {x,   y,   z},   {tex.back.x+tex.back.w, tex.back.y}, data}
        );
        DebugStats::triCount += 1;
    }
    
    if(adj & 4) {
        // LightData light = getLight(cx-1, y, cz);
        // LightData skyLight = getSkyLight(cx-1, y, cz);
        // unsigned int data = (skyLight << 4) | light;
        unsigned int data = getLightData(cx-1, y, cz);
        data |= BlockOrientation::WEST << 8;
        generateQuadMesh(mesh,
                { {x, y+h, z+1},   {tex.right.x+tex.right.w, tex.right.y+tex.right.h}, data},
                { {x, y+h, z},     {tex.right.x, tex.right.y+tex.right.h}, data},
                { {x, y,   z},     {tex.right.x, tex.right.y}, data},
                { {x, y,   z+1},   {tex.right.x+tex.right.w, tex.right.y}, data}
        );
        DebugStats::triCount += 1;
    }
    
    
    if(adj & 2) {
        // LightData light = getLight(cx, y-1, cz);
        // LightData skyLight = getSkyLight(cx, y-1, cz);
        // unsigned int data = (skyLight << 4) | light;
        unsigned int data = getLightData(cx, y-1, cz);
        data |= BlockOrientation::DOWN << 8;
        generateQuadMesh(mesh,
                { {x,   y, z+1},  {tex.bottom.x, tex.bottom.y+tex.bottom.h}, data},
                { {x,   y, z},    {tex.bottom.x, tex.bottom.y}, data},
                { {x+1, y, z},    {tex.bottom.x+tex.bottom.w, tex.bottom.y}, data},
                { {x+1, y, z+1},  {tex.bottom.x+tex.bottom.w, tex.bottom.y+tex.bottom.h}, data}
        );
        DebugStats::triCount += 1;
    }
}

void Chunk::generateTorchMesh(Mesh<Vertex> &mesh, int cx, int y, int cz, BlockTexture tex, unsigned char adj) {
    float x = pos.x + cx;
    float z = pos.y + cz;
    // LightData light = getLight(cx, y, cz);
    // LightData skyLight = getSkyLight(cx, y, cz);
    // unsigned int data = (skyLight << 4) | light;
    unsigned int data = getLightData(cx, y, cz);
    
    data &= 0x0FF;
    data |= BlockOrientation::NORTH << 8;
    generateQuadMesh(mesh,
        { {x,   y+1, z+9.f/16.f}, {tex.front.x, tex.front.y+tex.front.h}, data},
        { {x,   y,   z+9.f/16.f}, {tex.front.x, tex.front.y}, data},
        { {x+1, y,   z+9.f/16.f}, {tex.front.x+tex.front.w, tex.front.y}, data},
        { {x+1, y+1, z+9.f/16.f}, {tex.front.x+tex.front.w, tex.front.y+tex.front.h}, data}
    );
    DebugStats::triCount += 1;

    data &= 0x0FF;
    data |= BlockOrientation::EAST << 8;
    generateQuadMesh(mesh,
        { {x+9.f/16.f, y+1, z+1}, {tex.left.x, tex.left.y+tex.left.h}, data},
        { {x+9.f/16.f, y,   z+1}, {tex.left.x, tex.left.y}, data},
        { {x+9.f/16.f, y,   z},   {tex.left.x+tex.left.w, tex.left.y}, data},
        { {x+9.f/16.f, y+1, z},   {tex.left.x+tex.left.w, tex.left.y+tex.left.h}, data}
    );
    DebugStats::triCount += 1;

    data &= 0x0FF;
    data |= BlockOrientation::SOUTH << 8;
    generateQuadMesh(mesh,
            { {x,   y+1, z+7.f/16.f}, {tex.back.x+tex.back.w, tex.back.y+tex.back.h}, data},
            { {x+1, y+1, z+7.f/16.f}, {tex.back.x, tex.back.y+tex.back.h}, data},
            { {x+1, y,   z+7.f/16.f}, {tex.back.x, tex.back.y}, data},
            { {x,   y,   z+7.f/16.f}, {tex.back.x+tex.back.w, tex.back.y}, data}
    );
    DebugStats::triCount += 1;

    data &= 0x0FF;
    data |= BlockOrientation::WEST << 8;
    generateQuadMesh(mesh,
            { {x+7.f/16.f, y+1, z+1}, {tex.right.x+tex.right.w, tex.right.y+tex.right.h}, data},
            { {x+7.f/16.f, y+1, z},   {tex.right.x, tex.right.y+tex.right.h}, data},
            { {x+7.f/16.f, y,   z},   {tex.right.x, tex.right.y}, data},
            { {x+7.f/16.f, y,   z+1}, {tex.right.x+tex.right.w, tex.right.y}, data}
    );
    DebugStats::triCount += 1;

    data &= 0x0FF;
    data |= BlockOrientation::UP << 8;
    generateQuadMesh(mesh,
            { {x+9.f/16.f, y+10.f/16.f, z+7.f/16.f}, {tex.top.x+tex.top.w * 9.f/16.f, tex.top.y + tex.top.h * 8.f/16.f}, data},
            { {x+7.f/16.f, y+10.f/16.f, z+7.f/16.f}, {tex.top.x + tex.top.w * 7.f/16.f, tex.top.y + tex.top.h * 8.f/16.f}, data},
            { {x+7.f/16.f, y+10.f/16.f, z+9.f/16.f}, {tex.top.x + tex.top.w * 7.f/16.f, tex.top.y+tex.top.h * 10.f/16.f}, data},
            { {x+9.f/16.f, y+10.f/16.f, z+9.f/16.f}, {tex.top.x+tex.top.w * 9.f/16.f, tex.top.y+tex.top.h * 10.f/16.f}, data}
    );
    DebugStats::triCount += 1;

    // No bottom on torches
}

void Chunk::generateCrossMesh(Mesh<Vertex> &mesh, int cx, int y, int cz, BlockTexture tex, unsigned char adj) {
    float x = pos.x + cx;
    float z = pos.y + cz;
    float off = glm::sqrt(2.f) / 3.5f;
    // LightData light = getLight(cx, y, cz);
    // LightData skyLight = getSkyLight(cx, y, cz);
    // LightData lightData = (skyLight << 4) | light;
    unsigned int data = getLightData(cx, y, cz);
    data |= BlockOrientation::NORTH << 8;
    generateQuadMesh(mesh,
        { {x+.5f-off,   y+1, z+.5f - off}, {tex.front.x, tex.front.y+tex.front.h}, data},
        { {x+.5f-off,   y,   z+.5f - off}, {tex.front.x, tex.front.y}, data},
        { {x+.5f+off, y,   z+.5f + off}, {tex.front.x+tex.front.w, tex.front.y}, data},
        { {x+.5f+off, y+1, z+.5f + off}, {tex.front.x+tex.front.w, tex.front.y+tex.front.h}, data}
    );
    DebugStats::triCount += 1;

    generateQuadMesh(mesh,
        { {x+.5f-off,   y+1, z+.5f + off}, {tex.front.x, tex.front.y+tex.front.h}, data},
        { {x+.5f-off,   y,   z+.5f + off}, {tex.front.x, tex.front.y}, data},
        { {x+.5f+off, y,   z+.5f - off}, {tex.front.x+tex.front.w, tex.front.y}, data},
        { {x+.5f+off, y+1, z+.5f - off}, {tex.front.x+tex.front.w, tex.front.y+tex.front.h}, data}
    );
    DebugStats::triCount += 1;
}

bool transparentVisible(BlockID id, BlockID other) {
    return other != id && Blocks::getBlockFromID(other).transparent;
}

bool opaqueVisible(BlockID other) {
    return Blocks::getBlockFromID(other).transparent;
}

int Chunk::findMaxY() {
    BlockID id;
    for(int y = chunkH-1; y >= 0; y--)
        for(int x = 0; x < chunkW; x++) {
            memset(light[y][x], 0, chunkL);
            for(int z = 0; z < chunkL; z++) {
                id = blocks[y][x][z].id;
                if(id != Blocks::AIR_BLOCK && id != Blocks::NULL_BLOCK)
                    return y+1;
            }
        }
    return 0;
}

int Chunk::findMinY() {
    Block b;
    for(int y = 0; y < maxY; y++)
        for(int x = 0; x < chunkW; x++)
            for(int z = 0; z < chunkL; z++) {
                b = Blocks::getBlockFromID(blocks[y][x][z].id);
                if(b.transparent)
                    return y-1;
            }
    return chunkH;
}

// TODO: Either create a vector of Lights that contains all the lights that spread past the edge of the chunk, or
// make a way to grab a light value and start another calculateSkyLighting/calculateLighting from that position.
// TODO: Maybe have multiple meshes so that we only have to rebuild a portion of the chunk, as opposed to meshing
// the entire chunk. This may improve lighting performance.
void Chunk::rebuildMesh() {
    status = MESHING;
    AdjChunks chunks = World::getAdjacentChunks(pos);
    mesh.clear();
    waterMesh.clear();
    std::vector<Light> lights;
    BlockID id;
    Block b;
    maxY = findMaxY();
    minY = findMinY();
    if(maxY > chunkH) maxY = chunkH;
    if(minY < 0) minY = 0;
    for(int y = minY; y < maxY; y++)
        for(int x = 0; x < chunkW; x++) {
            memset(light[y][x], 0, chunkL);
            for(int z = 0; z < chunkL; z++) {
                b = Blocks::getBlockFromID(blocks[y][x][z].id);
                if(b.lightEmit > 0)
                    lights.push_back({{x, y, z}, b.lightEmit});
            }
        }
    // for(int y = 0; y < chunkH; y++)
    //     for(int x = 0; x < chunkW; x++) {
    //         memset(light[y][x], 0, chunkL);
    //         for(int z = 0; z < chunkL; z++) {
    //             id = blocks[y][x][z].id;
    //             b = Blocks::getBlockFromID(blocks[y][x][z]);
    //             if(id == Blocks::AIR_BLOCK || id == Blocks::NULL_BLOCK) {
                    
    //                 break;
    //             }
    //             if(( b.transparent && ( ( y == chunkH-1 || transparentVisible(id, blocks[y+1][x][z].id) )
    //                 || ( y != 0 && transparentVisible(id, blocks[y-1][x][z].id) )
    //                 || ( x == 0
    //                     ? chunks.left && transparentVisible(id, chunks.left->blocks[y][chunkW-1][z].id)
    //                     : transparentVisible(id, blocks[y][x-1][z].id) )
    //                 || ( x == chunkW-1
    //                     ? chunks.right && transparentVisible(id, chunks.right->blocks[y][0][z].id)
    //                     : transparentVisible(id, blocks[y][x+1][z].id) )
    //                 || ( z == chunkL-1
    //                     ? chunks.front && transparentVisible(id, chunks.front->blocks[y][x][0].id)
    //                     : transparentVisible(id, blocks[y][x][z+1].id) )
    //                 || ( z == 0
    //                     ? chunks.back && transparentVisible(id, chunks.back->blocks[y][x][chunkL-1].id)
    //                     : transparentVisible(id, blocks[y][x][z-1].id) ) ) ) ||
                    
    //                 (( y == chunkH-1 || opaqueVisible(blocks[y+1][x][z].id) )
    //                 || ( y != 0 && opaqueVisible(blocks[y-1][x][z].id) )
    //                 || ( x == 0
    //                     ? chunks.left && opaqueVisible(chunks.left->blocks[y][chunkW-1][z].id)
    //                     : opaqueVisible(blocks[y][x-1][z].id) )
    //                 || ( x == chunkW-1
    //                     ? chunks.right && opaqueVisible(chunks.right->blocks[y][0][z].id)
    //                     : opaqueVisible(blocks[y][x+1][z].id) )
    //                 || ( z == chunkL-1
    //                     ? chunks.front && opaqueVisible(chunks.front->blocks[y][x][0].id)
    //                     : opaqueVisible(blocks[y][x][z+1].id) )
    //                 || ( z == 0
    //                     ? chunks.back && opaqueVisible(chunks.back->blocks[y][x][chunkL-1].id)
    //                     : opaqueVisible(blocks[y][x][z-1].id) ) )) {
                    
    //                 if(y < minY) minY = y;
    //                 if(y > maxY) maxY = y;
                    
    //                 if(b.lightEmit > 0.f)
    //                     lights.push_back({{x, y, z}, b.lightEmit});
    //             }
    //         }
    //     }
    // for(int x = 0; x < chunkW; x++)
    //     for(int z = 0; z < chunkL; z++)
    calculateSkyLighting(0, maxY, 0, 15);
    
    for(const auto &p : lights)
        calculateLighting(p.pos.x, p.pos.y, p.pos.z, p.val);
    
    for(int y = minY; y <= maxY; y++) {
        for(int x = 0; x < chunkW; x++) {
            for(int z = 0; z < chunkL; z++) {
                id = blocks[y][x][z].id;
                if(id == Blocks::AIR_BLOCK || id == Blocks::NULL_BLOCK)
                    continue;
                
                b = Blocks::getBlockFromID(blocks[y][x][z].id);
                
                // 0bTBLRFB
                unsigned char adj = 0;
                switch(b.render) {
                    case CUBE:
                        if(b.id == Blocks::LEAVES)
                            generateCubeMesh(mesh, x, y, z, b.tex, 0xFF);
                        else if(b.transparent) {
                    case LIQUID:
                            adj = ( y == chunkH-1 ) || transparentVisible(id, blocks[y+1][x][z].id);
                            adj |= (y != 0 && transparentVisible(id, blocks[y-1][x][z].id)) << 1;
                            adj |= (( x == 0 )
                                ? chunks.left && transparentVisible(id, chunks.left->blocks[y][chunkW-1][z].id)
                                : transparentVisible(id, blocks[y][x-1][z].id)) << 2;
                            adj |= (( x == chunkW-1 )
                                ? chunks.right && transparentVisible(id, chunks.right->blocks[y][0][z].id)
                                : transparentVisible(id, blocks[y][x+1][z].id)) << 3;
                            adj |= (( z == chunkL-1 )
                                ? chunks.front && transparentVisible(id, chunks.front->blocks[y][x][0].id)
                                : transparentVisible(id, blocks[y][x][z+1].id)) << 4;
                            adj |= (( z == 0 )
                                ? chunks.back && transparentVisible(id, chunks.back->blocks[y][x][chunkL-1].id)
                                : transparentVisible(id, blocks[y][x][z-1].id)) << 5;
                            
                            if(adj) {
                                if(b.render == LIQUID)
                                    generateLiquidMesh(waterMesh, x, y, z, b.tex, adj);
                                else
                                    generateCubeMesh(mesh, x, y, z, b.tex, adj);
                            }
                            break;
                        } else {
                            adj = (y == chunkH-1 || opaqueVisible(blocks[y+1][x][z].id));
                            adj |= (y != 0 && opaqueVisible(blocks[y-1][x][z].id)) << 1;
                            adj |= (x == 0
                                ? chunks.left && opaqueVisible(chunks.left->blocks[y][chunkW-1][z].id)
                                : opaqueVisible(blocks[y][x-1][z].id)) << 2;
                            adj |= (x == chunkW-1
                                ? chunks.right && opaqueVisible(chunks.right->blocks[y][0][z].id)
                                : opaqueVisible(blocks[y][x+1][z].id)) << 3;
                            adj |= (z == chunkL-1
                                ? chunks.front && opaqueVisible(chunks.front->blocks[y][x][0].id)
                                : opaqueVisible(blocks[y][x][z+1].id)) << 4;
                            adj |= (z == 0
                                ? chunks.back && opaqueVisible(chunks.back->blocks[y][x][chunkL-1].id)
                                : opaqueVisible(blocks[y][x][z-1].id)) << 5;
                            
                            if(adj)
                                generateCubeMesh(mesh, x, y, z, b.tex, adj);
                        }
                        break;
                    case TORCH:
                        generateTorchMesh(mesh, x, y, z, b.tex, adj);
                        break;
                    case CROSS:
                        generateCrossMesh(waterMesh, x, y, z, b.tex, adj);
                        break;
                    default:
                        fprintf(stderr, "Unknown mesh type: value = %d", b.render);
                        break;
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

LightData Chunk::getLight(int x, int y, int z) {
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

LightData Chunk::getSkyLight(int x, int y, int z) {
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

void Chunk::setLight(int x, int y, int z, LightData value) {
    light[y][x][z] &= 0xF0;
    light[y][x][z] |= value & 0xF;
}

void Chunk::setSkyLight(int x, int y, int z, LightData value) {
    light[y][x][z] &= 0xF;
    light[y][x][z] |= value << 4;
}

LightData Chunk::getLightData(int x, int y, int z) {
    bool top = y == maxY+1;
    
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
    else if(y > Chunk::chunkH)
        y = Chunk::chunkH-1;
    
    LightData data = light[y][x][z];
    if(top) data |= 0xF0;
    return data;
}
