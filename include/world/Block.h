#ifndef MINECRAFT_CLONE_BLOCK_H
#define MINECRAFT_CLONE_BLOCK_H

#include "renderer/spriteSheet.h"

typedef unsigned char BlockID;

struct BlockTexture {
    BlockTexture(TexCoords allSides);
    BlockTexture(TexCoords top, TexCoords bottom, TexCoords side);
    BlockTexture(TexCoords top, TexCoords bottom, TexCoords left, TexCoords right, TexCoords front, TexCoords back);
    BlockTexture();
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
