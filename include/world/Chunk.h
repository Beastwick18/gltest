#ifndef MINECRAFT_CLONE_CHUNK_H
#define MINECRAFT_CLONE_CHUNK_H

#include "renderer/Batch.hpp"
#include "renderer/Frustum.h"
#include "world/Block.h"

enum ChunkStatus {
    EMPTY,
    BUILDING,
    BUILT,
    HIDDEN,
    MESHING,
    SHOWING
};


struct Light {
    glm::ivec3 pos;
    LightData val;
};

struct AdjChunks;
struct SurroundingBlocks;

class Chunk {
public:
    Chunk(int x, int z);
    Chunk();
    ~Chunk();
    
    void generateChunk();
    
    void calculateSkyLighting(int x, int y, int z, LightData prev, bool *modified, LightData ***light);
    void calculateLighting(int x, int y, int z, LightData prev, bool *modified, LightData ***light);
    void calculateSkyLighting(int x, int y, int z, LightData prev, bool *modified = NULL);
    void calculateLighting(int x, int y, int z, LightData prev, bool *modified = NULL);
    // void calculateSkyLightingQuick(int x = 0, int y = chunkH-1, int z = 0, LightData prev = 15);
    // void calculateLightingQuick(int x = 0, int y = 0, int z = 0, LightData prev = 15);
    // void calculateSkyLighting(LightData ***buffer, int x = 0, int y = chunkH-1, int z = 0, LightData prev = 15);
    // void calculateLighting(LightData ***buffer, int x = 0, int y = 0, int z = 0, LightData prev = 15);
    
    void generateQuadMesh(Mesh<Vertex> &mesh, Vertex v0, Vertex v1, Vertex v2, Vertex v3);
    void generateCubeMesh(Mesh<Vertex> &mesh, int x, int y, int z, BlockTexture tex, unsigned char adj);
    void generateLiquidMesh(Mesh<Vertex> &mesh, int x, int y, int z, BlockTexture tex, unsigned char adj);
    void generateTorchMesh(Mesh<Vertex> &mesh, int x, int y, int z, BlockTexture tex);
    void generateCrossMesh(Mesh<Vertex> &mesh, int x, int y, int z, BlockTexture tex);
    void fullRebuildMesh();
    void rebuildMesh();
    void clearLighting();
    void clearBleedLighting();
    void recalculateLighting();
    void recalculateFullBleedLighting();
    BlockID getBlock(int x, int y, int z) const;
    void addBlock(BlockID id, int x, int y, int z);
    void removeBlock(int x, int y, int z);
    void show();
    void hide();
    bool isVisible(const Frustum *f) const;
    bool operator<(const Chunk& other) const;
    static float getNoise(glm::vec2 position);
    static float getNoise(float x, float z);
    inline const Mesh<Vertex>& getMesh() const { return mesh; }
    inline const Mesh<Vertex>& getWaterMesh() const { return waterMesh; }
    inline glm::ivec2 getPos() const { return pos; }
    inline ChunkStatus getStatus() const { return status; }
    inline bool isDirty() { return dirty; }
    inline void setDirty(bool dan) { dirty = dan; }
    LightData getLight(int x, int y, int z);
    LightData getSkyLight(int x, int y, int z);
    LightData getLightData(int x, int y, int z);
    // LightData getLight(LightData ***buffer, int x, int y, int z);
    // LightData getSkyLight(LightData ***buffer, int x, int y, int z);
    // LightData getLightData(LightData ***buffer, int x, int y, int z);
    
    void setLight(LightData ***light, int x, int y, int z, LightData value);
    void setSkyLight(LightData ***light, int x, int y, int z, LightData value);
    // void setLight(LightData ***buffer, int x, int y, int z, LightData value);
    // void setSkyLight(LightData ***buffer, int x, int y, int z, LightData value);
    
    int findMaxY();
    int findMinY();
    void findMaxMin();
    
    SurroundingBlocks getAdjacentBlocks(AdjChunks &chunks, const int x, const int y, const int z);
    static bool transparentVisible(BlockID id, BlockID other);
    static bool opaqueVisible(BlockID other);
    static unsigned char transparentVisible(BlockID id, SurroundingBlocks s);
    static unsigned char opaqueVisible(SurroundingBlocks s);
    
    static const int chunkW = 16, chunkL = 16, chunkH = 256;
    // static const int chunkW = 8, chunkL = 8, chunkH = 150;
    // static const int chunkW = 16, chunkL = 16, chunkH = 256;
    // static const int chunkW = 64, chunkL = 64, chunkH = 256;
private:
    glm::ivec2 pos;
    Mesh<Vertex> mesh;
    // Mesh<Vertex> transparentMesh;
    Mesh<Vertex> waterMesh;
    ChunkStatus status;
    BlockData blocks[chunkH][chunkW][chunkL];
    
    bool dirty;
    
    // Change to unsigned int. Then put color data in the other
    // 3 bytes. Send this to the shader to create colored lighting
    // (maybe idk if this is a good idea)
    LightData ***light;
    LightData ***lightBleed;
    // LightData light[chunkH][chunkW][chunkL];
    // LightData lightBleed[chunkH][chunkW][chunkL];
    int maxY, minY;
};

struct AdjChunks {
    Chunk *left;
    Chunk *right;
    Chunk *front;
    Chunk *back;
};

#endif
