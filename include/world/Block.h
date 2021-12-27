#ifndef MINECRAFT_CLONE_BLOCK_H
#define MINECRAFT_CLONE_BLOCK_H

#include "renderer/texture2D.h"

typedef unsigned char BlockID;

struct BlockTexture {
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
    
    bool operator==(const Block &other) {
        return id == other.id;
    }
};

namespace Blocks {
    extern Block blocks[256];
    extern BlockID airBlockID;
    extern Block airBlock;
    extern BlockID nullBlockID;
    extern Block nullBlock;
    
    Block getBlockFromID(BlockID id);
    BlockID getIdFromName(std::string name);
}

#endif
