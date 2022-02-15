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
    Block airBlock { "Air", {}, 0, true, false, false, false, 0.f, 0.f };
    BlockID nullBlockID = 255;
    Block nullBlock { "Null", {}, 255, true, false, false, false, 0.f, 0.f };
    SpriteSheet *blockAtlas = nullptr;
    BlockTexture highlight;
    
    // TODO: Load this from a config file
    void init() {
        blockAtlas = SpriteSheet::loadFromImageFile("assets/textures/block_atlas.png", 32, 32);
        TexCoords null = blockAtlas->getSubTexture(29, 14);
        Blocks::nullBlock.tex = null;
        std::fill(blocks, blocks+256, nullBlock);
        
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
        TexCoords bedrock = blockAtlas->getSubTexture(4, 28);
        TexCoords cobblestone = blockAtlas->getSubTexture(3, 26);
        TexCoords planks = blockAtlas->getSubTexture(8, 16);
        TexCoords glowstone = blockAtlas->getSubTexture(14, 22);
        TexCoords torch = blockAtlas->getSubTexture(20, 24);
        TexCoords redstoneTorch = blockAtlas->getSubTexture(18, 31);
        TexCoords glass = blockAtlas->getSubTexture(15, 27);
        
        // TexCoords square = blockAtlas->getSubTexture(30, 13, 2, 2);
        TexCoords white = blockAtlas->getSubTexture(29, 13);
        highlight = { white };
        
        Blocks::blocks[0] = Blocks::airBlock;
        Blocks::blocks[1] = Block{ "Grass", { grassTop, dirt, grassSide }, 1, false, false, true, false, 1.0f, 0.f };
        Blocks::blocks[2] = Block{ "Dirt", { dirt }, 2, false, false, true, false, 1.0f, 0.f };
        Blocks::blocks[3] = Block{ "Stone", { stone }, 3, false, false, true, false, 1.0f, 0.f };
        Blocks::blocks[4] = Block{ "Log", { logTop, logTop, logSide }, 4, false, false, true, false, 1.0f, 0.f };
        Blocks::blocks[5] = Block{ "Leaves", { leaves }, 5, true, false, true, false, 1.0f, 0.f };
        Blocks::blocks[6] = Block{ "Sand", { sand }, 6, false, false, true, false, 1.0f, 0.f };
        Blocks::blocks[7] = Block{ "Water", { water }, 7, true, true, true, false, 0.2f, 0.f };
        Blocks::blocks[8] = Block{ "Glowstone", { glowstone }, 8, false, false, true, false, 1.0f, 1.f };
        Blocks::blocks[9] = Block{ "Bedrock", { bedrock }, 9, false, false, false, false, 1.0f, 0.f };
        Blocks::blocks[10] = Block{ "Cobblestone", { cobblestone }, 10, false, false, true, false, 1.0f, 0.f };
        Blocks::blocks[11] = Block{ "WoodenPlanks", { planks }, 11, false, false, true, false, 1.0f, 0.f };
        Blocks::blocks[12] = Block{ "Lava", { lava }, 12, true, true, true, false, 0.0f, 1.f };
        Blocks::blocks[13] = Block{ "Torch", { torch }, 13, true, false, true, false, 0.0f, 1.f };
        Blocks::blocks[14] = Block{ "RedstoneTorch", { redstoneTorch }, 14, true, false, true, false, 0.0f, 0.5f };
        Blocks::blocks[15] = Block{ "Glass", { glass }, 15, true, false, true, false, 0.0f, 0.0f };
    }
    
    void free() {
        SpriteSheet::free(blockAtlas);
    }
    
    const Block &getBlockFromID(BlockID id) {
        // if(id >= 0 && id <= 255)
            return blocks[id];
        // return nullBlock;
    }
    
    BlockID getIdFromName(std::string name) {
        for(const auto &b : blocks)
            if(b.name == name)
                return b.id;
        return nullBlockID;
    }
}
