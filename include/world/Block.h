#ifndef MINECRAFT_CLONE_BLOCK_H
#define MINECRAFT_CLONE_BLOCK_H

#include "renderer/spriteSheet.h"

typedef uint8_t BlockID;
typedef uint8_t LightData;

enum BlockOrientation : unsigned char {
    WEST    = 0,
    EAST    = 1,
    UP      = 2,
    DOWN    = 3,
    NORTH   = 4,
    SOUTH   = 5
};

struct BlockData {
    BlockID id;
    BlockOrientation orientation;
    LightData light;
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
    float lightBlocking = 1.f;
    float lightEmit = 0.f;
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
    };
    extern Block blocks[numBlocks];
    extern SpriteSheet *blockAtlas;
    extern BlockTexture highlight;
    
    void init();
    void free();
    const Block &getBlockFromID(BlockID id);
    BlockID getIdFromName(std::string name);
}

#endif
