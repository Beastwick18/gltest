#include "renderer/renderer.h"
#include "renderer/cubemap.h"
#include "renderer/shader.h"
#include "input/input.h"
#include "glm/gtc/constants.hpp"
#include "renderer/EBO.h"

namespace Renderer {
    Shader *regularShader, *transparentShader, *cubemapShader, *crosshairShader;
    GLint vpUniform, wVpUniform;
    GLint waveUniform;
    GLint sunUniform, wSunUniform;
    GLint cmView, cmProj, cmTime;
    Batch<Vertex> regularBatch;
    Batch<Vertex> transparentBatch;
    float skyBrightness = 1.f;
    
    VAO *cmVao;
    VBO *cmVbo;
    CubeMap *m;
    
    VAO *cVao;
    VBO *cVbo;
    EBO *cEbo;
    
    const Camera *camera;
    const MinecraftClone::Window *window;
    
    float wave = 0;
    
    static const float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    
    void init(MinecraftClone::Window *window) {
        Renderer::window = window;
        camera = nullptr;
        
        cVao = new VAO;
        cVao->bind();
        cVbo = new VBO(16*sizeof(float));
        VBlayout cLayout;
        cLayout.push<float>(2); // Push position
        cVao->addBuffer(cVbo, cLayout);
        cEbo = new EBO(12);
        cEbo->bind();
        
        crosshairShader = Shader::createShader("assets/shaders/quad.glsl");
        
        VBlayout cmLayout;
        cmLayout.push<float>(3);
        cmVao = new VAO;
        cmVbo = new VBO(skyboxVertices, sizeof(skyboxVertices), GL_STATIC_DRAW);
        cmVao->addBuffer(cmVbo, cmLayout);
        
        VBlayout layout;
        layout.push<float>(3);
        layout.push<float>(3);
        layout.push<float>(2);
        layout.push<float>(1);
        layout.push<float>(1);
        regularBatch.init(layout);
        transparentBatch.init(layout);
        
        std::vector<std::string> files = {
            "assets/textures/skybox/right.jpg",
            "assets/textures/skybox/left.jpg",
            "assets/textures/skybox/top.jpg",
            "assets/textures/skybox/bottom.jpg",
            "assets/textures/skybox/front.jpg",
            "assets/textures/skybox/back.jpg",
        };
        m = CubeMap::loadFromImageFile(files);
        cubemapShader = Shader::createShader("assets/shaders/skybox.glsl");
        cmView = cubemapShader->getUniformLocation("view");
        cmProj = cubemapShader->getUniformLocation("projection");
        cmTime = cubemapShader->getUniformLocation("time");
        
        
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
        VAO::free(cmVao);
        VBO::free(cmVbo);
        VAO::free(cVao);
        VBO::free(cVbo);
        EBO::free(cEbo);
        CubeMap::free(m);
        Shader::freeShader(cubemapShader);
        Shader::freeShader(crosshairShader);
    }
    
    void setCamera(const Camera* cameraPtr) {
        camera = cameraPtr;
    }
    
    void generateQuadMesh(Mesh<Vertex> &newMesh, Vertex v0, Vertex v1, Vertex v2, Vertex v3) {
        newMesh.addVertex(v0);
        newMesh.addVertex(v1);
        newMesh.addVertex(v2);
        newMesh.addVertex(v2);
        newMesh.addVertex(v3);
        newMesh.addVertex(v0);
    }

    void generateCubeMesh(Mesh<Vertex> &mesh, glm::vec3 pos, BlockTexture tex, SurroundingBlocks surr, float light, float skyLight) {
        generateCubeMesh(mesh, pos.x, pos.y, pos.z, tex, surr.top, surr.bottom, surr.left, surr.right, surr.front, surr.back, light, skyLight);
    }
    
    void generateCubeMesh(Mesh<Vertex> &mesh, float x, float y, float z, BlockTexture tex, bool top, bool bottom, bool left, bool right, bool front, bool back, float light, float skyLight) {
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
    
    void generateLiquidMesh(Mesh<Vertex> &mesh, glm::vec3 pos, BlockTexture tex, SurroundingBlocks surr, float light, float skyLight) {
        generateLiquidMesh(mesh, pos.x, pos.y, pos.z, tex, surr.top, surr.bottom, surr.left, surr.right, surr.front, surr.back, light, skyLight);
    }
    
    void generateLiquidMesh(Mesh<Vertex> &mesh, float x, float y, float z, BlockTexture tex, bool top, bool bottom, bool left, bool right, bool front, bool back, float light, float skyLight) {
        float h = 1.f;
        if(top) {
            tex.left.h *= 15.f/16.f;
            tex.right.h *= 15.f/16.f;
            tex.front.h *= 15.f/16.f;
            tex.back.h *= 15.f/16.f;
            h = 15.f/16.f;
            generateQuadMesh(mesh,
                    { {x+1, y+h, z},   {0, 1, 0}, {tex.top.x+tex.top.w, tex.top.y}, light, skyLight },
                    { {x,   y+h, z},   {0, 1, 0}, {tex.top.x, tex.top.y}, light, skyLight },
                    { {x,   y+h, z+1}, {0, 1, 0}, {tex.top.x, tex.top.y+tex.top.h}, light, skyLight },
                    { {x+1, y+h, z+1}, {0, 1, 0}, {tex.top.x+tex.top.w, tex.top.y+tex.top.h}, light, skyLight }
            );
            DebugStats::triCount += 1;
        }
        
        if(front) {
            generateQuadMesh(mesh,
                { {x,   y+h, z+1}, {0, 0, -1}, {tex.front.x, tex.front.y+tex.front.h}, light, skyLight },
                { {x,   y,   z+1}, {0, 0, -1}, {tex.front.x, tex.front.y}, light, skyLight },
                { {x+1, y,   z+1}, {0, 0, -1}, {tex.front.x+tex.front.w, tex.front.y}, light, skyLight },
                { {x+1, y+h, z+1}, {0, 0, -1}, {tex.front.x+tex.front.w, tex.front.y+tex.front.h}, light, skyLight }
            );
            DebugStats::triCount += 1;
        }
        
        if(right) {
            generateQuadMesh(mesh,
                { {x+1, y+h, z+1}, {-1, 0, 0}, {tex.left.x, tex.left.y+tex.left.h}, light, skyLight },
                { {x+1, y,   z+1}, {-1, 0, 0}, {tex.left.x, tex.left.y}, light, skyLight },
                { {x+1, y,   z},   {-1, 0, 0}, {tex.left.x+tex.left.w, tex.left.y}, light, skyLight },
                { {x+1, y+h, z},   {-1, 0, 0}, {tex.left.x+tex.left.w, tex.left.y+tex.left.h}, light, skyLight }
            );
            DebugStats::triCount += 1;
        }
        
        if(back) {
            generateQuadMesh(mesh,
                    { {x,   y+h, z},   {0, 0, 1}, {tex.back.x+tex.back.w, tex.back.y+tex.back.h}, light, skyLight },
                    { {x+1, y+h, z},   {0, 0, 1}, {tex.back.x, tex.back.y+tex.back.h}, light, skyLight },
                    { {x+1, y,   z},   {0, 0, 1}, {tex.back.x, tex.back.y}, light, skyLight },
                    { {x,   y,   z},   {0, 0, 1}, {tex.back.x+tex.back.w, tex.back.y}, light, skyLight }
            );
            DebugStats::triCount += 1;
        }
        
        if(left) {
            generateQuadMesh(mesh,
                    { {x, y+h, z+1},   {1, 0, 0}, {tex.right.x+tex.right.w, tex.right.y+tex.right.h}, light, skyLight },
                    { {x, y+h, z},     {1, 0, 0}, {tex.right.x, tex.right.y+tex.right.h}, light, skyLight },
                    { {x, y,   z},     {1, 0, 0}, {tex.right.x, tex.right.y}, light, skyLight },
                    { {x, y,   z+1},   {1, 0, 0}, {tex.right.x+tex.right.w, tex.right.y}, light, skyLight }
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
        
    void generateTorchMesh(Mesh<Vertex> &mesh, float x, float y, float z, BlockTexture tex, SurroundingBlocks adj, float light, float skyLight) {
        if(adj.front) {
            generateQuadMesh(mesh,
                { {x,   y+1, z+9.f/16.f}, {0, 0, -1}, {tex.front.x, tex.front.y+tex.front.h}, light, skyLight },
                { {x,   y,   z+9.f/16.f}, {0, 0, -1}, {tex.front.x, tex.front.y}, light, skyLight },
                { {x+1, y,   z+9.f/16.f}, {0, 0, -1}, {tex.front.x+tex.front.w, tex.front.y}, light, skyLight },
                { {x+1, y+1, z+9.f/16.f}, {0, 0, -1}, {tex.front.x+tex.front.w, tex.front.y+tex.front.h}, light, skyLight }
            );
            DebugStats::triCount += 1;
        }
        
        if(adj.right) {
            generateQuadMesh(mesh,
                { {x+9.f/16.f, y+1, z+1}, {-1, 0, 0}, {tex.left.x, tex.left.y+tex.left.h}, light, skyLight },
                { {x+9.f/16.f, y,   z+1}, {-1, 0, 0}, {tex.left.x, tex.left.y}, light, skyLight },
                { {x+9.f/16.f, y,   z},   {-1, 0, 0}, {tex.left.x+tex.left.w, tex.left.y}, light, skyLight },
                { {x+9.f/16.f, y+1, z},   {-1, 0, 0}, {tex.left.x+tex.left.w, tex.left.y+tex.left.h}, light, skyLight }
            );
            DebugStats::triCount += 1;
        }
        
        if(adj.back) {
            generateQuadMesh(mesh,
                    { {x,   y+1, z+7.f/16.f},   {0, 0, 1}, {tex.back.x+tex.back.w, tex.back.y+tex.back.h}, light, skyLight },
                    { {x+1, y+1, z+7.f/16.f},   {0, 0, 1}, {tex.back.x, tex.back.y+tex.back.h}, light, skyLight },
                    { {x+1, y,   z+7.f/16.f},   {0, 0, 1}, {tex.back.x, tex.back.y}, light, skyLight },
                    { {x,   y,   z+7.f/16.f},   {0, 0, 1}, {tex.back.x+tex.back.w, tex.back.y}, light, skyLight }
            );
            DebugStats::triCount += 1;
        }
        
        if(adj.left) {
            generateQuadMesh(mesh,
                    { {x+7.f/16.f, y+1, z+1},   {1, 0, 0}, {tex.right.x+tex.right.w, tex.right.y+tex.right.h}, light, skyLight },
                    { {x+7.f/16.f, y+1, z},     {1, 0, 0}, {tex.right.x, tex.right.y+tex.right.h}, light, skyLight },
                    { {x+7.f/16.f, y,   z},     {1, 0, 0}, {tex.right.x, tex.right.y}, light, skyLight },
                    { {x+7.f/16.f, y,   z+1},   {1, 0, 0}, {tex.right.x+tex.right.w, tex.right.y}, light, skyLight }
            );
            DebugStats::triCount += 1;
        }
        
        if(adj.top) {
            generateQuadMesh(mesh,
                    { {x+9.f/16.f, y+10.f/16.f, z+7.f/16.f},   {0, 1, 0}, {tex.top.x+tex.top.w * 9.f/16.f, tex.top.y + tex.top.w * 8.f/16.f}, light, skyLight },
                    { {x+7.f/16.f,   y+10.f/16.f, z+7.f/16.f},   {0, 1, 0}, {tex.top.x + tex.top.w * 7.f/16.f, tex.top.y + tex.top.w * 8.f/16.f}, light, skyLight },
                    { {x+7.f/16.f,   y+10.f/16.f, z+9.f/16.f}, {0, 1, 0}, {tex.top.x + tex.top.w * 7.f/16.f, tex.top.y+tex.top.h * 10.f/16.f}, light, skyLight },
                    { {x+9.f/16.f, y+10.f/16.f, z+9.f/16.f}, {0, 1, 0}, {tex.top.x+tex.top.w * 9.f/16.f, tex.top.y+tex.top.h * 10.f/16.f}, light, skyLight }
            );
            DebugStats::triCount += 1;
        }
        
        // // No bottom on torches
        // if(adj.bottom) {
        //     float light = getLight(cx, y-1, cz);
        //     float skyLight = getSkyLight(cx, y-1, cz);
        //     generateQuadMesh(mesh,
        //             { {x,   y, z+1}, {0, -1, 0}, {tex.bottom.x, tex.bottom.y+tex.bottom.h}, light, skyLight },
        //             { {x,   y, z},   {0, -1, 0}, {tex.bottom.x, tex.bottom.y}, light, skyLight },
        //             { {x+1, y, z},   {0, -1, 0}, {tex.bottom.x+tex.bottom.w, tex.bottom.y}, light, skyLight },
        //             { {x+1, y, z+1}, {0, -1, 0}, {tex.bottom.x+tex.bottom.w, tex.bottom.y+tex.bottom.h}, light, skyLight }
        //     );
        //     DebugStats::triCount += 1;
        // }
    }

    
    void flushRegularBatch() {
        regularShader->use();
        regularShader->setUniform1f(sunUniform, skyBrightness);
        regularShader->setUniformMat4f(vpUniform, camera->getProjection() * camera->getView());
        regularBatch.flush();
    }
    
    void flushTransparentBatch() {
        if(transparentBatch.isEmpty())
            return;
        
        // glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        // transparentShader->use();
        transparentShader->use();
        transparentShader->setUniform1f(wSunUniform, skyBrightness);
        transparentShader->setUniform1f(waveUniform, wave);
        transparentShader->setUniformMat4f(wVpUniform, camera->getProjection() * camera->getView());
        transparentBatch.flush();
        // glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
    }

    void renderMesh(const Vertex *mesh, const size_t size) {
        if(!regularBatch.hasRoomFor(size)) {
            flushRegularBatch();
            if(!regularBatch.hasRoomFor(size)) {
                fprintf(stderr, "Mesh too big for batch\n");
                return;
            }
        }
        regularBatch.addVertices(mesh, size);
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
    
    void renderCubemap() {
        glDepthMask(GL_FALSE);
        cubemapShader->use();
        cubemapShader->setUniformMat4f(cmView, glm::mat4(glm::mat3(camera->getView())));
        cubemapShader->setUniformMat4f(cmProj, camera->getProjection());
        cubemapShader->setUniform1f(cmTime, Renderer::skyBrightness);
        cmVao->bind();
        m->bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
    }
    
    void renderCrosshair() {
        crosshairShader->use();
        float x = 14.f/ MinecraftClone::Input::window->getWidth();
        float y = 2.f/  MinecraftClone::Input::window->getHeight();
        float x2 = 2.f/ MinecraftClone::Input::window->getWidth();
        float y2 = 14.f/MinecraftClone::Input::window->getHeight();
        float vertices[] = {
            -x,  y, // Top left
             x,  y, // Top right
             x, -y, // Bottom right
            -x, -y,  // Bottom left
            -x2,  y2, // Top left
             x2,  y2, // Top right
             x2, -y2, // Bottom right
            -x2, -y2  // Bottom left
        };
        static const unsigned int indices[] = {
            0, 2, 1,
            2, 0, 3,
            4, 6, 5,
            6, 4, 7
        };
        
        cVao->bind();
        cEbo->bind();
        glDrawElements(GL_TRIANGLES, cEbo->getCount(), GL_UNSIGNED_INT, nullptr);
        
        cVbo->bind();
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        cEbo->bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_ARRAY_BUFFER, 0, 8);
        
    }
    
    void update(double deltaTime) {
        wave += (float)deltaTime*1.5f;
        if(wave > 2*glm::pi<float>())

            wave = 0;
        if(MinecraftClone::Input::isKeyDown(GLFW_KEY_R))
            printf("%f\n",wave);
    }
}
