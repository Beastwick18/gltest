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
    void calculateSkyLighting(int x = 0, int y = chunkH-1, int z = 0, LightData prev = 15);
    void calculateSkyLightingSpread(int x = 0, int y = chunkH-1, int z = 0, LightData prev = 15);
    void calculateLighting(int x = 0, int y = 0, int z = 0, LightData prev = 15);
    void generateQuadMesh(Mesh<Vertex> &mesh, Vertex v0, Vertex v1, Vertex v2, Vertex v3);
    void generateCubeMesh(Mesh<Vertex> &mesh, int x, int y, int z, BlockTexture tex, unsigned char adj);
    void generateLiquidMesh(Mesh<Vertex> &mesh, int x, int y, int z, BlockTexture tex, unsigned char adj);
    void generateTorchMesh(Mesh<Vertex> &mesh, int x, int y, int z, BlockTexture tex, unsigned char adj);
    void generateCrossMesh(Mesh<Vertex> &mesh, int x, int y, int z, BlockTexture tex, unsigned char adj);
    void rebuildMesh();
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
    LightData getLight(int x, int y, int z);
    LightData getSkyLight(int x, int y, int z);
    LightData getLightData(int x, int y, int z);
    void setLight(int x, int y, int z, LightData value);
    void setSkyLight(int x, int y, int z, LightData value);
    int findMaxY();
    int findMinY();
    static const int chunkW = 32, chunkL = 32, chunkH = 256;
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
    
    // Change to unsigned int. Then put color data in the other
    // 3 bytes. Send this to the shader to create colored lighting
    // (maybe idk if this is a good idea)
    LightData light[chunkH][chunkW][chunkL];
    int maxY, minY;
};

struct AdjChunks {
    Chunk *left;
    Chunk *right;
    Chunk *front;
    Chunk *back;
};

#endif
