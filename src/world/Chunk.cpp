// TODO: Create a chunk mesh of vertices that should be sent to the renderer for rendering. Only update the mesh when something changes

#include <mutex>
#include "world/Chunk.h"
#include "glm/gtc/noise.hpp"
#include "renderer/texture2D.h"

Chunk::Chunk(int x, int y) : pos(x,y) {
    status = ChunkStatus::EMPTY;
}

Chunk::~Chunk() {
    
}

float Chunk::getNoise(float x, float z) {
    glm::vec2 position((float)x/100.f, (float)z/100.f);
    float h = (1+glm::simplex(position))/3.0 * 50;
    
    float mid = (glm::simplex(glm::vec2(x/20.f,z/20.f)))/2.0 * 5;
    h += mid;
    
    float ext = (glm::simplex(glm::vec2(x/10.f, z/10.f))) * 1.f;
    h += ext;
    
    return h;
}

void Chunk::generateChunk() {
    
}

void Chunk::rebuildMesh() {
    // TexCoords grass { 1.f/32.f, 21.f/32.f, 1.f/32.f, 1.f/32.f };
    // TexCoords grassTop { 4.f/32.f, 21.f/32.f, 1.f/32.f, 1.f/32.f };
    // TexCoords dirt { 8.f/32.f, 26.f/32.f, 1.f/32.f, 1.f/32.f };
    // TexCoords grassBottom = dirt;
    // TexCoords stone { 19.f/32.f, 25.f/32.f, 1.f/32.f, 1.f/32.f };
    
    // int w = pos.x + chunkW, l = pos.y +chunkL, height;
    // for(int x = pos.x; x < w; x++) {
    //     for(int z = pos.y; z < l; z++) {
    //         height = getNoise(x, z)+10;
    //         float heightLeft = getNoise(x+1, z)+10;
    //         float heightRight = getNoise(x-1, z)+10;
    //         float heightFront = getNoise(x, z+1)+10;
    //         float heightBack = getNoise(x, z-1)+10;
    //         float stoneHeight = (getNoise(x, z)+10) / 1.15f;
    //         for(int y = 0; y < height; y++) {
    //             if(y < stoneHeight-1) {
    //                 generateCube(quads,
    //                         x, y, z, stone, stone, stone, 
    //                         y == height-1, false, heightLeft < y+1, heightRight < y+1,
    //                         heightFront < y+1, heightBack < y+1
    //                 );
    //             } else if(y < height-1) {
    //                 generateCube(quads,
    //                         x, y, z, dirt, dirt, dirt,
    //                         y == height-1, false, heightLeft < y+1, heightRight < y+1,
    //                         heightFront < y+1, heightBack < y+1
    //                 );
    //             } else {
    //                 generateCube(quads,
    //                         x, y, z, grassTop, dirt, grass, 
    //                         y == height-1, false, heightLeft < y+1, heightRight < y+1,
    //                         heightFront < y+1, heightBack < y+1
    //                 );
    //             }
    //         }
    //     }
    // }
    
    // ChunkMesh mesh;
    // // mesh.size = quads.size() * 6;
    // // mesh.v = (Vertex *)calloc(mesh.size, sizeof(Vertex));
    // // mesh.v.push_back(
    // mesh.chunkX = (float)startx/16.f;
    // mesh.chunkZ = (float)startz/16.f;
    // for(const auto &q : quads)
    //     for(const auto &v : q.vertices)
    //         mesh.v.push_back(v);
    // std::lock_guard<std::mutex> lock(BatchScene3D::meshesMutex);
    // meshes->push_back(mesh);
    // DebugStats::triCount += quads.size() * 3;
}
