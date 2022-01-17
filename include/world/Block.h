#ifndef MINECRAFT_CLONE_BLOCK_H
#define MINECRAFT_CLONE_BLOCK_H

#include "renderer/spriteSheet.h"

typedef unsigned char BlockID;

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
    bool transparent;
    bool liquid;
    bool breakable;
    bool rotatable;
};

namespace Blocks {
    extern Block blocks[256];
    extern BlockID airBlockID;
    extern Block airBlock;
    extern BlockID nullBlockID;
    extern Block nullBlock;
    extern SpriteSheet *blockAtlas;
    extern BlockTexture highlight;
    
    void init();
    void free();
    const Block &getBlockFromID(BlockID id);
    BlockID getIdFromName(std::string name);
}

#endif
