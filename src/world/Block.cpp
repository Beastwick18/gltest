// TODO: Each block rendered IS NOT another instance of a class. Each block class will have info about the block, plus a list of all occurences of the block
// within the view distance.

#include "world/Block.h"

namespace Blocks {
    Block blocks[256];
    BlockID airBlockID = 0;
    Block airBlock { "Air", {}, 0, true };
    BlockID nullBlockID = 255;
    Block nullBlock { "Null", {}, 0, true };
    
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
