#ifndef MINECRAFT_CLONE_BLOCK_H
#define MINECRAFT_CLONE_BLOCK_H

#include "renderer/Batch.hpp"
#include "renderer/spriteSheet.h"

typedef uint8_t BlockID;
typedef uint8_t LightData;

enum BlockOrientation : unsigned int {
    WEST    = 3,
    EAST    = 0,
    UP      = 1,
    DOWN    = 4,
    NORTH   = 5,
    SOUTH   = 2
};

struct BlockData {
    BlockID id;
    BlockOrientation orientation;
};


struct BlockTexture {
    BlockTexture(TexCoords allSides);
    BlockTexture(TexCoords top, TexCoords bottom, TexCoords side);
    BlockTexture(TexCoords top, TexCoords bottom, TexCoords left, TexCoords right, TexCoords front, TexCoords back);
    BlockTexture();
    
    // Store all rotated versions of the texcoords
    TexCoords top;
    TexCoords bottom;
    TexCoords left;
    TexCoords right;
    TexCoords front;
    TexCoords back;
};

struct Block {
    std::string name;
    BlockTexture tex;
    BlockID id;
    bool liquid = false;
    bool transparent = false;
    bool breakable = true;
    bool rotatable = false;
    LightData lightBlocking = 15;
    LightData lightEmit = 0;
    MeshType render = MeshType::CUBE;
};

namespace Blocks {
    static const size_t numBlocks = 1 << (sizeof(BlockID) * 8);
    
    enum: BlockID {
        NULL_BLOCK = numBlocks - 1,
        AIR_BLOCK = 0,
        GRASS = 1,
        DIRT = 2,
        STONE = 3,
        LOG = 4,
        LEAVES = 5,
        SAND = 6,
        WATER = 7,
        GLOWSTONE = 8,
        BEDROCK = 9,
        COBBLESTONE = 10,
        WOODEN_PLANKS = 11,
        LAVA = 12,
        TORCH = 13,
        REDSTONE_TORCH = 14,
        GLASS = 15,
        TALL_GRASS = 16,
        VERY_TALL_GRASS = 17,
        VERY_TALL_GRASS_TOP = 18,
        ROSE = 19,
        DANDELION = 20,
        WHITE_TULIP = 21,
        RED_MUSHROOM = 22,
        BROWN_MUSHROOM = 23,
        SUGAR_CANE = 24,
    };
    extern Block blocks[numBlocks];
    extern SpriteSheet *blockAtlas;
    extern BlockTexture highlight;
    
    void init();
    void free();
    const Block &getBlockFromID(BlockID id);
    BlockID getIdFromName(const std::string &name);
}

#endif
