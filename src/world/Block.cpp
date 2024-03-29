#include "world/Block.h"

BlockTexture::BlockTexture(TexCoords allSides) : top(allSides), bottom(allSides), left(allSides), right(allSides), front(allSides), back(allSides) {}
BlockTexture::BlockTexture(TexCoords top, TexCoords bottom, TexCoords side) : top(top), bottom(bottom), left(side), right(side), front(side), back(side) {}
BlockTexture::BlockTexture(TexCoords top, TexCoords bottom, TexCoords left, TexCoords right, TexCoords front, TexCoords back) : top(top), bottom(bottom), left(left), right(right), front(front), back(back) {}
BlockTexture::BlockTexture() {}

namespace Blocks {
    Block blocks[numBlocks];
    SpriteSheet *blockAtlas = nullptr, *blockAtlas1 = nullptr, *blockAtlas2 = nullptr, *blockAtlas3 = nullptr;
    BlockTexture highlight;
    
    // TODO: Load this from a config file
    void init() {
        blockAtlas = SpriteSheet::loadFromImageFile("assets/textures/block_atlas_tp2.png", 32, 32);
        blockAtlas1 = SpriteSheet::loadFromImageFile("assets/textures/block_atlas_tp2-1.png", 32, 32);
        blockAtlas2 = SpriteSheet::loadFromImageFile("assets/textures/block_atlas_tp2-2.png", 32, 32);
        blockAtlas3 = SpriteSheet::loadFromImageFile("assets/textures/block_atlas_tp2-3.png", 32, 32);
        blockAtlas->bind(0);
        blockAtlas1->bind(1);
        blockAtlas2->bind(2);
        blockAtlas3->bind(3);
        TexCoords null = blockAtlas->getSubTexture(29, 14);
        Block nullBlock { .name = "Null", .tex = null, .id = NULL_BLOCK, .transparent = false };
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
        TexCoords tallGrass = blockAtlas->getSubTexture(20, 28);
        TexCoords veryTallGrass = blockAtlas->getSubTexture(10, 31);
        TexCoords veryTallGrassTop = blockAtlas->getSubTexture(10, 30);
        TexCoords rose = blockAtlas->getSubTexture(14, 28);
        TexCoords dandelion = blockAtlas->getSubTexture(13, 24);
        TexCoords whiteTulip = blockAtlas->getSubTexture(14, 24);
        TexCoords sugarCane = blockAtlas->getSubTexture(18, 30);
        TexCoords redMushroom = blockAtlas->getSubTexture(5, 17);
        TexCoords brownMushroom = blockAtlas->getSubTexture(4, 17);
        
        TexCoords white = blockAtlas->getSubTexture(29, 13);
        highlight = { white };
        
        blocks[AIR_BLOCK] = { .name = "Air", .id = AIR_BLOCK, .transparent = true };
        blocks[GRASS] = Block{ .name = "Grass", .tex = { grassTop, dirt, grassSide }, .id = GRASS };
        blocks[DIRT] = Block{ .name = "Dirt", .tex = { dirt }, .id = DIRT };
        blocks[STONE] = Block{ .name = "Stone", .tex = { stone }, .id = STONE };
        blocks[LOG] = Block{ .name = "Log", .tex = { logTop, logTop, logSide }, .id = LOG };
        blocks[LEAVES] = Block{ .name = "Leaves", .tex = { leaves }, .id = LEAVES, .transparent = true, .lightBlocking = 1 };
        blocks[SAND] = Block{ .name = "Sand", .tex = { sand }, .id = SAND, };
        blocks[WATER] = Block{ .name = "Water", .tex = { water }, .id = WATER, .liquid = true, .transparent = true, .lightBlocking = 1, .lightEmit = 0, .render = LIQUID };
        blocks[GLOWSTONE] = Block{ .name = "Glowstone", .tex = { glowstone }, .id = GLOWSTONE, .lightEmit = 15 };
        blocks[BEDROCK] = Block{ .name = "Bedrock", .tex = { bedrock }, .id = BEDROCK, .breakable = false };
        blocks[COBBLESTONE] = Block{ .name = "Cobblestone", .tex = { cobblestone }, .id = COBBLESTONE };
        blocks[WOODEN_PLANKS] = Block{ .name = "WoodenPlanks", .tex = { planks }, .id = WOODEN_PLANKS };
        blocks[LAVA] = Block{ .name = "Lava", .tex = { lava }, .id = LAVA, .liquid = true, .transparent = true, .lightEmit = 15, .render = LIQUID };
        blocks[TORCH] = Block{ .name = "Torch", .tex = { torch }, .id = TORCH, .transparent = true, .lightBlocking = 0, .lightEmit = 15, .render = MeshType::TORCH };
        blocks[REDSTONE_TORCH] = Block{ .name = "RedstoneTorch", .tex = { redstoneTorch }, .id = REDSTONE_TORCH, .transparent = true, .lightEmit = 7, .render = MeshType::TORCH };
        blocks[GLASS] = Block{ .name = "Glass", .tex = { glass }, .id = GLASS, .transparent = true, .lightBlocking = 0 };
        blocks[TALL_GRASS] = Block{ .name = "TallGrass", .tex = { tallGrass }, .id = TALL_GRASS, .transparent = true, .lightBlocking = 0, .render = CROSS };
        blocks[VERY_TALL_GRASS] = Block{ .name = "VeryTallGrass", .tex = { veryTallGrass }, .id = TALL_GRASS, .transparent = true, .lightBlocking = 0, .render = CROSS };
        blocks[VERY_TALL_GRASS_TOP] = Block{ .name = "VeryTallGrassTop", .tex = { veryTallGrassTop }, .id = TALL_GRASS, .transparent = true, .lightBlocking = 0, .render = CROSS };
        blocks[ROSE] = Block{ .name = "Rose", .tex = { rose }, .id = ROSE, .transparent = true, .lightBlocking = 0, .render = CROSS };
        blocks[DANDELION] = Block{ .name = "Dandelion", .tex = { dandelion }, .id = DANDELION, .transparent = true, .lightBlocking = 0, .render = CROSS };
        blocks[WHITE_TULIP] = Block{ .name = "WhiteTulip", .tex = { whiteTulip }, .id = WHITE_TULIP, .transparent = true, .lightBlocking = 0, .render = CROSS };
        blocks[RED_MUSHROOM] = Block{ .name = "SugarCane", .tex = { redMushroom }, .id = RED_MUSHROOM, .transparent = true, .lightBlocking = 0, .render = CROSS };
        blocks[BROWN_MUSHROOM] = Block{ .name = "SugarCane", .tex = { brownMushroom }, .id = BROWN_MUSHROOM, .transparent = true, .lightBlocking = 0, .render = CROSS };
        blocks[SUGAR_CANE] = Block{ .name = "SugarCane", .tex = { sugarCane }, .id = SUGAR_CANE, .transparent = true, .lightBlocking = 0, .render = CROSS };
    }
    
    void free() {
        SpriteSheet::free(blockAtlas);
        SpriteSheet::free(blockAtlas1);
        SpriteSheet::free(blockAtlas2);
        SpriteSheet::free(blockAtlas3);
    }
    
    const Block &getBlockFromID(BlockID id) {
        return blocks[id];
    }
    
    BlockID getIdFromName(const std::string &name) {
        for(const auto &b : blocks)
            if(b.name == name)
                return b.id;
        return NULL_BLOCK;
    }
}
