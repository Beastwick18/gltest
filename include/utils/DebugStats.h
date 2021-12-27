#ifndef MINECRAFT_CLONE_DEBUG_STATS_H
#define MINECRAFT_CLONE_DEBUG_STATS_H

namespace DebugStats {
    extern float renderTime, drawTime, updateTime, guiTime, flushTime;
    extern unsigned int triCount, drawCalls, chunksRenderedCount, frameCount, updateCount;
    
    void reset();
}

#endif
