// TODO: Each block rendered IS NOT another instance of a class. Each block class will have info about the block, plus a list of all occurences of the block
// within the view distance.

#include "world/Block.h"

BlockTexture::BlockTexture(TexCoords allSides) : top(allSides), bottom(allSides), left(allSides), right(allSides), front(allSides), back(allSides) {}
BlockTexture::BlockTexture(TexCoords top, TexCoords bottom, TexCoords side) : top(top), bottom(bottom), left(side), right(side), front(side), back(side) {}
BlockTexture::BlockTexture(TexCoords top, TexCoords bottom, TexCoords left, TexCoords right, TexCoords front, TexCoords back) : top(top), bottom(bottom), left(left), right(right), front(front), back(back) {}
BlockTexture::BlockTexture() {}

namespace Blocks {
    Block blocks[256];
    BlockID airBlockID = 0;
    Block airBlock { "Air", {}, 0, true, false };
    BlockID nullBlockID = 255;
    Block nullBlock { "Null", {}, 0, true, true };
    // SpriteSheet *blockAtlas = SpriteSheet::loadFromImageFile("assets/textures/block_atlas.png", 32, 32);
    SpriteSheet * blockAtlas = nullptr;
    BlockTexture highlight;
    
    void init() {
        blockAtlas = SpriteSheet::loadFromImageFile("assets/textures/block_atlas.png", 32, 32);
        
        TexCoords grassSide = blockAtlas->getSubTexture(1, 21);
        TexCoords grassTop = blockAtlas->getSubTexture(4, 21);
        TexCoords dirt = blockAtlas->getSubTexture(8, 26);
        TexCoords stone = blockAtlas->getSubTexture(19, 25);
        TexCoords logSide = blockAtlas->getSubTexture(7, 18);
        TexCoords logTop = blockAtlas->getSubTexture(8, 18);
        TexCoords leaves = blockAtlas->getSubTexture(9, 19);
        TexCoords sand = blockAtlas->getSubTexture(18, 27);
        TexCoords water = blockAtlas->getSubTexture(20, 19);
        TexCoords lava = blockAtlas->getSubTexture(8, 19);
        
        TexCoords square = blockAtlas->getSubTexture(30, 13, 2, 2);
        highlight = { square };
        
        Blocks::blocks[0] = Blocks::airBlock;
        Blocks::blocks[1] = Block{ "Grass", { grassTop, dirt, grassSide }, 1, false, false };
        Blocks::blocks[2] = Block{ "Dirt", { dirt }, 2, false, false };
        Blocks::blocks[3] = Block{ "Stone", { stone }, 3, false, false };
        Blocks::blocks[4] = Block{ "Log", { logTop, logTop, logSide }, 4, false, false };
        Blocks::blocks[5] = Block{ "Leaves", { leaves }, 5, true, false };
        Blocks::blocks[6] = Block{ "Sand", { sand }, 6, false, false };
        Blocks::blocks[7] = Block{ "Water", { water }, 7, true, true };
        Blocks::blocks[8] = Block{ "Lava", { lava }, 8, true, true };
        Blocks::blocks[255] = Blocks::nullBlock;
    }
    
    void free() {
        SpriteSheet::free(blockAtlas);
    }
    
    const Block &getBlockFromID(BlockID id) {
        if(id < 0 || id > 255)
            return nullBlock;
        return blocks[id];
    }
    
    BlockID getIdFromName(std::string name) {
        for(const auto &b : blocks)
            if(b.name == name)
                return b.id;
        return nullBlockID;
    }
}
