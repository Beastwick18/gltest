#ifndef MINECRAFT_CLONE_BLOCK_H
#define MINECRAFT_CLONE_BLOCK_H

#include "renderer/texture2D.h"

typedef unsigned char BlockID;

struct BlockTexture {
    BlockTexture(TexCoords allSides) : top(allSides), bottom(allSides), left(allSides), right(allSides), front(allSides), back(allSides) {}
    BlockTexture(TexCoords top, TexCoords bottom, TexCoords side) : top(top), bottom(bottom), left(side), right(side), front(side), back(side) {}
    BlockTexture(TexCoords top, TexCoords bottom, TexCoords left, TexCoords right, TexCoords front, TexCoords back) : top(top), bottom(bottom), left(left), right(right), front(front), back(back) {}
    BlockTexture() {}
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
    
    const Block &getBlockFromID(BlockID id);
    BlockID getIdFromName(std::string name);
}

#endif
