#include "renderer/renderer.h"
#include "renderer/shader.h"
#include "input/input.h"

namespace Renderer {
    Shader *regularShader, *transparentShader;
    GLint vpUniform, wVpUniform;
    GLint waveUniform;
    GLint sunUniform, wSunUniform;
    Batch<Vertex> regularBatch;
    Batch<Vertex> transparentBatch;
    float skyBrightness = 1.f;
    
    static const Camera *camera;
    
    float wave = 0;
    
    void init() {
        camera = nullptr;
        VBlayout layout;
        layout.push<float>(3);
        layout.push<float>(3);
        layout.push<float>(2);
        layout.push<float>(1);
        layout.push<float>(1);
        regularBatch.init(layout);
        transparentBatch.init(layout);
        
        regularShader = Shader::createShader("assets/shaders/batch3d.glsl");
        regularShader->use();
        regularShader->setUniform1i("tex0", 0);
        regularShader->setUniformMat4f("model", glm::mat4(1.f));
        sunUniform = regularShader->getUniformLocation("skyBrightness");
        vpUniform = regularShader->getUniformLocation("viewProj");
        
        transparentShader = Shader::createShader("assets/shaders/waveyBlock.glsl");
        transparentShader->use();
        transparentShader->setUniform1i("tex0", 0);
        transparentShader->setUniformMat4f("model", glm::mat4(1.f));
        wSunUniform = transparentShader->getUniformLocation("skyBrightness");
        wVpUniform = transparentShader->getUniformLocation("viewProj");
        waveUniform = transparentShader->getUniformLocation("waveOffset");
    }
    
    void free() {
        Shader::freeShader(regularShader);
        Shader::freeShader(transparentShader);
        regularBatch.free();
        transparentBatch.free();
    }
    
    void setCamera(const Camera* cameraPtr) {
        camera = cameraPtr;
    }
    
    void generateQuadMesh(std::vector<Vertex> &newMesh, Vertex v0, Vertex v1, Vertex v2, Vertex v3) {
        newMesh.push_back(v0);
        newMesh.push_back(v1);
        newMesh.push_back(v2);
        newMesh.push_back(v2);
        newMesh.push_back(v3);
        newMesh.push_back(v0);
    }

    void generateCubeMesh(std::vector<Vertex> &mesh, float x, float y, float z, BlockTexture tex, bool top, bool bottom, bool left, bool right, bool front, bool back, float light, float skyLight) {
        if(front) {
            generateQuadMesh(mesh,
                { {x,   y+1, z+1}, {0, 0, -1}, {tex.front.x, tex.front.y+tex.front.h}, light, skyLight },
                { {x,   y,   z+1}, {0, 0, -1}, {tex.front.x, tex.front.y}, light, skyLight },
                { {x+1, y,   z+1}, {0, 0, -1}, {tex.front.x+tex.front.w, tex.front.y}, light, skyLight },
                { {x+1, y+1, z+1}, {0, 0, -1}, {tex.front.x+tex.front.w, tex.front.y+tex.front.h}, light, skyLight }
            );
            DebugStats::triCount += 1;
        }
        
        if(right) {
            generateQuadMesh(mesh,
                { {x+1, y+1, z+1}, {-1, 0, 0}, {tex.left.x, tex.left.y+tex.left.h}, light, skyLight },
                { {x+1, y,   z+1}, {-1, 0, 0}, {tex.left.x, tex.left.y}, light, skyLight },
                { {x+1, y,   z},   {-1, 0, 0}, {tex.left.x+tex.left.w, tex.left.y}, light, skyLight },
                { {x+1, y+1, z},   {-1, 0, 0}, {tex.left.x+tex.left.w, tex.left.y+tex.left.h}, light, skyLight }
            );
            DebugStats::triCount += 1;
        }
        
        if(back) {
            generateQuadMesh(mesh,
                    { {x,   y+1, z},   {0, 0, 1}, {tex.back.x+tex.back.w, tex.back.y+tex.back.h}, light, skyLight },
                    { {x+1, y+1, z},   {0, 0, 1}, {tex.back.x, tex.back.y+tex.back.h}, light, skyLight },
                    { {x+1, y,   z},   {0, 0, 1}, {tex.back.x, tex.back.y}, light, skyLight },
                    { {x,   y,   z},   {0, 0, 1}, {tex.back.x+tex.back.w, tex.back.y}, light, skyLight }
            );
            DebugStats::triCount += 1;
        }
        
        if(left) {
            generateQuadMesh(mesh,
                    { {x, y+1, z+1},   {1, 0, 0}, {tex.right.x+tex.right.w, tex.right.y+tex.right.h}, light, skyLight },
                    { {x, y+1, z},     {1, 0, 0}, {tex.right.x, tex.right.y+tex.right.h}, light, skyLight },
                    { {x, y,   z},     {1, 0, 0}, {tex.right.x, tex.right.y}, light, skyLight },
                    { {x, y,   z+1},   {1, 0, 0}, {tex.right.x+tex.right.w, tex.right.y}, light, skyLight }
            );
            DebugStats::triCount += 1;
        }
        
        if(top) {
            generateQuadMesh(mesh,
                    { {x+1, y+1, z},   {0, 1, 0}, {tex.top.x+tex.top.w, tex.top.y}, light, skyLight },
                    { {x,   y+1, z},   {0, 1, 0}, {tex.top.x, tex.top.y}, light, skyLight },
                    { {x,   y+1, z+1}, {0, 1, 0}, {tex.top.x, tex.top.y+tex.top.h}, light, skyLight },
                    { {x+1, y+1, z+1}, {0, 1, 0}, {tex.top.x+tex.top.w, tex.top.y+tex.top.h}, light, skyLight }
            );
            DebugStats::triCount += 1;
        }
        
        if(bottom) {
            generateQuadMesh(mesh,
                    { {x,   y, z+1}, {0, -1, 0}, {tex.bottom.x, tex.bottom.y+tex.bottom.h}, light, skyLight },
                    { {x,   y, z},   {0, -1, 0}, {tex.bottom.x, tex.bottom.y}, light, skyLight },
                    { {x+1, y, z},   {0, -1, 0}, {tex.bottom.x+tex.bottom.w, tex.bottom.y}, light, skyLight },
                    { {x+1, y, z+1}, {0, -1, 0}, {tex.bottom.x+tex.bottom.w, tex.bottom.y+tex.bottom.h}, light, skyLight }
            );
            DebugStats::triCount += 1;
        }
    }
    
    void flushRegularBatch() {
        regularShader->use();
        transparentShader->setUniform1f(sunUniform, skyBrightness);
        regularShader->setUniformMat4f(vpUniform, camera->getProjection() * camera->getView());
        regularBatch.flush();
    }
    
    void flushTransparentBatch() {
        if(transparentBatch.isEmpty())
            return;
        
        // glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        transparentShader->use();
        transparentShader->setUniform1f(wSunUniform, skyBrightness);
        transparentShader->setUniform1f(waveUniform, wave);
        transparentShader->setUniformMat4f(wVpUniform, camera->getProjection() * camera->getView());
        transparentBatch.flush();
        // glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
    }

    void renderMesh(const Vertex *mesh, const size_t size) {
        if(!regularBatch.hasRoomFor(size))
            flushRegularBatch();
        
        if(regularBatch.hasRoomFor(size))
            regularBatch.addVertices(mesh, size);
        else
            fprintf(stderr, "Mesh too big for batch\n");
    }

    void renderTransparentMesh(const Vertex *mesh, const size_t size) {
        if(!transparentBatch.hasRoomFor(size))
            flushTransparentBatch();
        if(!regularBatch.isEmpty()) // Ensure we're rendering over opaque blocks
            flushRegularBatch();
        
        if(transparentBatch.hasRoomFor(size))
            transparentBatch.addVertices(mesh, size);
        else
            fprintf(stderr, "Mesh too big for batch\n");
    }

    void render() {
        flushRegularBatch();
        flushTransparentBatch();
    }
    
    void update(double deltaTime) {
        wave += (float)deltaTime*1.5f;
        if(wave > 2*3.14f)
            wave = 0;
        if(MinecraftClone::Input::isKeyDown(GLFW_KEY_R))
            printf("%f\n",wave);
    }
}
