#include "utils/DebugStats.h"

namespace DebugStats {
    float renderTime = 0, drawTime = 0, updateTime = 0, guiTime = 0, flushTime = 0;
    unsigned int triCount = 0, drawCalls = 0, chunksRenderedCount = 0, frameCount = 0, updateCount = 0;
    
    void reset() {
        renderTime = 0;
        updateTime = 0;
        updateCount = 0;
        guiTime = 0;
        drawTime = 0;
        flushTime = 0;
        chunksRenderedCount = 0;
        drawCalls = 0;
    }
}
