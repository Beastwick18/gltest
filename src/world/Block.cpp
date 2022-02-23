#include "world/Block.h"

BlockTexture::BlockTexture(TexCoords allSides) : top(allSides), bottom(allSides), left(allSides), right(allSides), front(allSides), back(allSides) {}
BlockTexture::BlockTexture(TexCoords top, TexCoords bottom, TexCoords side) : top(top), bottom(bottom), left(side), right(side), front(side), back(side) {}
BlockTexture::BlockTexture(TexCoords top, TexCoords bottom, TexCoords left, TexCoords right, TexCoords front, TexCoords back) : top(top), bottom(bottom), left(left), right(right), front(front), back(back) {}
BlockTexture::BlockTexture() {}

namespace Blocks {
    Block blocks[numBlocks];
    SpriteSheet *blockAtlas = nullptr;
    BlockTexture highlight;
    
    // TODO: Load this from a config file
    void init() {
        blockAtlas = SpriteSheet::loadFromImageFile("assets/textures/block_atlas_tp2.png", 32, 32);
        TexCoords null = blockAtlas->getSubTexture(29, 14);
        Block nullBlock { .name = "Null", .tex = null, .id = NULL_BLOCK, .transparent = true };
        std::fill(blocks, blocks+numBlocks, nullBlock);
        
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
        
        blocks[AIR_BLOCK] = { .name = "Air", .id = AIR_BLOCK, .transparent = true };
        blocks[GRASS] = Block{ .name = "Grass", .tex = { grassTop, dirt, grassSide }, .id = GRASS };
        blocks[DIRT] = Block{ .name = "Dirt", .tex = { dirt }, .id = DIRT };
        blocks[STONE] = Block{ .name = "Stone", .tex = { stone }, .id = STONE };
        blocks[LOG] = Block{ .name = "Log", .tex = { logTop, logTop, logSide }, .id = LOG };
        blocks[LEAVES] = Block{ .name = "Leaves", .tex = { leaves }, .id = LEAVES, .transparent = true };
        blocks[SAND] = Block{ .name = "Sand", .tex = { sand }, .id = SAND, };
        blocks[WATER] = Block{ .name = "Water", .tex = { water }, .id = WATER, .liquid = true, .transparent = true, .lightBlocking = 0.2f };
        blocks[GLOWSTONE] = Block{ .name = "Glowstone", .tex = { glowstone }, .id = GLOWSTONE, .lightEmit = 1.f };
        blocks[BEDROCK] = Block{ .name = "Bedrock", .tex = { bedrock }, .id = BEDROCK, .breakable = false };
        blocks[COBBLESTONE] = Block{ .name = "Cobblestone", .tex = { cobblestone }, .id = COBBLESTONE };
        blocks[WOODEN_PLANKS] = Block{ .name = "WoodenPlanks", .tex = { planks }, .id = WOODEN_PLANKS };
        blocks[LAVA] = Block{ .name = "Lava", .tex = { lava }, .id = LAVA, .liquid = true, .transparent = true, .lightEmit = 1.f };
        blocks[TORCH] = Block{ .name = "Torch", .tex = { torch }, .id = TORCH, .transparent = true, .lightEmit = 1.f };
        blocks[REDSTONE_TORCH] = Block{ .name = "RedstoneTorch", .tex = { redstoneTorch }, .id = REDSTONE_TORCH, .transparent = true, .lightEmit = 0.5f };
        blocks[GLASS] = Block{ .name = "Glass", .tex = { glass }, .id = GLASS, .transparent = true };
    }
    
    void free() {
        SpriteSheet::free(blockAtlas);
    }
    
    const Block &getBlockFromID(BlockID id) {
        return blocks[id];
    }
    
    BlockID getIdFromName(std::string name) {
        for(const auto &b : blocks)
            if(b.name == name)
                return b.id;
        return NULL_BLOCK;
    }
}
