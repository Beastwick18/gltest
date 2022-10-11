#include "renderer/renderer.h"
#include "renderer/cubemap.h"
#include "renderer/shader.h"
#include "input/input.h"
#include "glm/gtc/constants.hpp"
#include "renderer/EBO.h"

namespace Renderer {
    Shader *regularShader, *transparentShader, *cubemapShader, *crosshairShader;
    GLint viewUniform, wViewUniform, projUniform, wProjUniform;
    GLint texUniform, wTexUniform;
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
    int frame = 0;
    
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
        // layout.push<float>(3);
        layout.push<float>(2);
        layout.push<int>(1);
        // layout.push<float>(1);
        // layout.push<float>(1);
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
        regularShader->setUniform1i("selTex", 0);
        regularShader->setUniformMat4f("model", glm::mat4(1.f));
        sunUniform = regularShader->getUniformLocation("skyBrightness");
        viewUniform = regularShader->getUniformLocation("view");
        projUniform = regularShader->getUniformLocation("proj");
        texUniform = regularShader->getUniformLocation("selTex");
        
        transparentShader = Shader::createShader("assets/shaders/waveyBlock.glsl");
        transparentShader->use();
        transparentShader->setUniform1i("selTex", 0);
        transparentShader->setUniformMat4f("model", glm::mat4(1.f));
        wSunUniform = transparentShader->getUniformLocation("skyBrightness");
        wViewUniform = transparentShader->getUniformLocation("view");
        wProjUniform = transparentShader->getUniformLocation("proj");
        waveUniform = transparentShader->getUniformLocation("waveOffset");
        wTexUniform = regularShader->getUniformLocation("selTex");
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
        Vertex arr[4] = {v0, v1, v2, v3};
        GLuint idc[6] = {0, 1, 2, 2, 3 ,0};
        newMesh.addVertices(arr, 4, idc, 6);
        // newMesh.addVertex(v0);
        // newMesh.addVertex(v1);
        // newMesh.addVertex(v2);
        // newMesh.addVertex(v2);
        // newMesh.addVertex(v3);
        // newMesh.addVertex(v0);
    }

    void generateCubeMesh(Mesh<Vertex> &mesh, glm::vec3 pos, BlockTexture tex, SurroundingBlocks surr, LightData light) {
        generateCubeMesh(mesh, pos.x, pos.y, pos.z, tex, surr.top, surr.bottom, surr.left, surr.right, surr.front, surr.back, light);
    }
    
    void generateCubeMesh(Mesh<Vertex> &mesh, float x, float y, float z, BlockTexture tex, bool top, bool bottom, bool left, bool right, bool front, bool back, LightData light) {
        if(front) {
            unsigned int data = light | (BlockOrientation::SOUTH << 8);
            generateQuadMesh(mesh,
                { {x,   y+1, z+1}, {tex.front.x, tex.front.y+tex.front.h}, data },
                { {x,   y,   z+1}, {tex.front.x, tex.front.y}, data },
                { {x+1, y,   z+1}, {tex.front.x+tex.front.w, tex.front.y}, data },
                { {x+1, y+1, z+1}, {tex.front.x+tex.front.w, tex.front.y+tex.front.h}, data }
            );
            DebugStats::triCount += 1;
        }
        
        if(right) {
            unsigned int data = light | (BlockOrientation::EAST << 8);
            generateQuadMesh(mesh,
                { {x+1, y+1, z+1}, {tex.left.x, tex.left.y+tex.left.h}, data },
                { {x+1, y,   z+1}, {tex.left.x, tex.left.y}, data },
                { {x+1, y,   z},   {tex.left.x+tex.left.w, tex.left.y}, data },
                { {x+1, y+1, z},   {tex.left.x+tex.left.w, tex.left.y+tex.left.h}, data }
            );
            DebugStats::triCount += 1;
        }
        
        if(back) {
            unsigned int data = light | (BlockOrientation::NORTH << 8);
            generateQuadMesh(mesh,
                    { {x,   y+1, z},   {tex.back.x+tex.back.w, tex.back.y+tex.back.h}, data },
                    { {x+1, y+1, z},   {tex.back.x, tex.back.y+tex.back.h}, data },
                    { {x+1, y,   z},   {tex.back.x, tex.back.y}, data },
                    { {x,   y,   z},   {tex.back.x+tex.back.w, tex.back.y}, data }
            );
            DebugStats::triCount += 1;
        }
        
        if(left) {
            unsigned int data = light | (BlockOrientation::WEST << 8);
            generateQuadMesh(mesh,
                    { {x, y+1, z+1},   {tex.right.x+tex.right.w, tex.right.y+tex.right.h}, data },
                    { {x, y+1, z},     {tex.right.x, tex.right.y+tex.right.h}, data },
                    { {x, y,   z},     {tex.right.x, tex.right.y}, data },
                    { {x, y,   z+1},   {tex.right.x+tex.right.w, tex.right.y}, data }
            );
            DebugStats::triCount += 1;
        }
        
        if(top) {
            unsigned int data = light | (BlockOrientation::UP << 8);
            generateQuadMesh(mesh,
                    { {x+1, y+1, z},   {tex.top.x+tex.top.w, tex.top.y}, data },
                    { {x,   y+1, z},   {tex.top.x, tex.top.y}, data },
                    { {x,   y+1, z+1}, {tex.top.x, tex.top.y+tex.top.h}, data },
                    { {x+1, y+1, z+1}, {tex.top.x+tex.top.w, tex.top.y+tex.top.h}, data }
            );
            DebugStats::triCount += 1;
        }
        
        if(bottom) {
            unsigned int data = light | (BlockOrientation::DOWN << 8);
            generateQuadMesh(mesh,
                    { {x,   y, z+1},  {tex.bottom.x, tex.bottom.y+tex.bottom.h}, data },
                    { {x,   y, z},    {tex.bottom.x, tex.bottom.y}, data },
                    { {x+1, y, z},    {tex.bottom.x+tex.bottom.w, tex.bottom.y}, data },
                    { {x+1, y, z+1},  {tex.bottom.x+tex.bottom.w, tex.bottom.y+tex.bottom.h}, data }
            );
            DebugStats::triCount += 1;
        }
    }
    
    void generateLiquidMesh(Mesh<Vertex> &mesh, glm::vec3 pos, BlockTexture tex, SurroundingBlocks surr, LightData light) {
        generateLiquidMesh(mesh, pos.x, pos.y, pos.z, tex, surr.top, surr.bottom, surr.left, surr.right, surr.front, surr.back, light);
    }
    
    void generateLiquidMesh(Mesh<Vertex> &mesh, float x, float y, float z, BlockTexture tex, bool top, bool bottom, bool left, bool right, bool front, bool back, LightData light) {
        float h = 1.f;
        if(top) {
            tex.left.h *= 15.f/16.f;
            tex.right.h *= 15.f/16.f;
            tex.front.h *= 15.f/16.f;
            tex.back.h *= 15.f/16.f;
            h = 15.f/16.f;
            unsigned int data = light | (BlockOrientation::UP << 8);
            generateQuadMesh(mesh,
                    { {x+1, y+h, z},   {tex.top.x+tex.top.w, tex.top.y}, data },
                    { {x,   y+h, z},   {tex.top.x, tex.top.y}, data },
                    { {x,   y+h, z+1}, {tex.top.x, tex.top.y+tex.top.h}, data },
                    { {x+1, y+h, z+1}, {tex.top.x+tex.top.w, tex.top.y+tex.top.h}, data }
            );
            DebugStats::triCount += 1;
        }
        
        if(front) {
            unsigned int data = light | (BlockOrientation::SOUTH << 8);
            generateQuadMesh(mesh,
                { {x,   y+h, z+1},  {tex.front.x, tex.front.y+tex.front.h}, data },
                { {x,   y,   z+1},  {tex.front.x, tex.front.y}, data },
                { {x+1, y,   z+1},  {tex.front.x+tex.front.w, tex.front.y}, data },
                { {x+1, y+h, z+1},  {tex.front.x+tex.front.w, tex.front.y+tex.front.h}, data }
            );
            DebugStats::triCount += 1;
        }
        
        if(right) {
            unsigned int data = light | (BlockOrientation::EAST << 8);
            generateQuadMesh(mesh,
                { {x+1, y+h, z+1},  {tex.left.x, tex.left.y+tex.left.h}, data },
                { {x+1, y,   z+1},  {tex.left.x, tex.left.y}, data },
                { {x+1, y,   z},    {tex.left.x+tex.left.w, tex.left.y}, data },
                { {x+1, y+h, z},    {tex.left.x+tex.left.w, tex.left.y+tex.left.h}, data }
            );
            DebugStats::triCount += 1;
        }
        
        if(back) {
            unsigned int data = light | (BlockOrientation::NORTH << 8);
            generateQuadMesh(mesh,
                    { {x,   y+h, z},   {tex.back.x+tex.back.w, tex.back.y+tex.back.h}, data },
                    { {x+1, y+h, z},   {tex.back.x, tex.back.y+tex.back.h}, data },
                    { {x+1, y,   z},   {tex.back.x, tex.back.y}, data },
                    { {x,   y,   z},   {tex.back.x+tex.back.w, tex.back.y}, data }
            );
            DebugStats::triCount += 1;
        }
        
        if(left) {
            unsigned int data = light | (BlockOrientation::WEST << 8);
            generateQuadMesh(mesh,
                    { {x, y+h, z+1},   {tex.right.x+tex.right.w, tex.right.y+tex.right.h}, data },
                    { {x, y+h, z},     {tex.right.x, tex.right.y+tex.right.h}, data },
                    { {x, y,   z},     {tex.right.x, tex.right.y}, data },
                    { {x, y,   z+1},   {tex.right.x+tex.right.w, tex.right.y}, data }
            );
            DebugStats::triCount += 1;
        }
        
        
        if(bottom) {
            unsigned int data = light | (BlockOrientation::DOWN << 8);
            generateQuadMesh(mesh,
                    { {x,   y, z+1},  {tex.bottom.x, tex.bottom.y+tex.bottom.h}, data },
                    { {x,   y, z},    {tex.bottom.x, tex.bottom.y}, data },
                    { {x+1, y, z},    {tex.bottom.x+tex.bottom.w, tex.bottom.y}, data },
                    { {x+1, y, z+1},  {tex.bottom.x+tex.bottom.w, tex.bottom.y+tex.bottom.h}, data }
            );
            DebugStats::triCount += 1;
        }

    }
        
    void generateTorchMesh(Mesh<Vertex> &mesh, float x, float y, float z, BlockTexture tex, SurroundingBlocks adj, LightData light) {
        if(adj.front) {
            unsigned int data = light | (BlockOrientation::SOUTH << 8);
            generateQuadMesh(mesh,
                { {x,   y+1, z+9.f/16.f},  {tex.front.x, tex.front.y+tex.front.h}, data },
                { {x,   y,   z+9.f/16.f},  {tex.front.x, tex.front.y}, data },
                { {x+1, y,   z+9.f/16.f},  {tex.front.x+tex.front.w, tex.front.y}, data },
                { {x+1, y+1, z+9.f/16.f},  {tex.front.x+tex.front.w, tex.front.y+tex.front.h}, data }
            );
            DebugStats::triCount += 1;
        }
        
        if(adj.right) {
            unsigned int data = light | (BlockOrientation::EAST << 8);
            generateQuadMesh(mesh,
                { {x+9.f/16.f, y+1, z+1},  {tex.left.x, tex.left.y+tex.left.h}, data },
                { {x+9.f/16.f, y,   z+1},  {tex.left.x, tex.left.y}, data },
                { {x+9.f/16.f, y,   z},    {tex.left.x+tex.left.w, tex.left.y}, data },
                { {x+9.f/16.f, y+1, z},    {tex.left.x+tex.left.w, tex.left.y+tex.left.h}, data }
            );
            DebugStats::triCount += 1;
        }
        
        if(adj.back) {
            unsigned int data = light | (BlockOrientation::NORTH << 8);
            generateQuadMesh(mesh,
                    { {x,   y+1, z+7.f/16.f},   {tex.back.x+tex.back.w, tex.back.y+tex.back.h}, data },
                    { {x+1, y+1, z+7.f/16.f},   {tex.back.x, tex.back.y+tex.back.h}, data },
                    { {x+1, y,   z+7.f/16.f},   {tex.back.x, tex.back.y}, data },
                    { {x,   y,   z+7.f/16.f},   {tex.back.x+tex.back.w, tex.back.y}, data }
            );
            DebugStats::triCount += 1;
        }
        
        if(adj.left) {
            unsigned int data = light | (BlockOrientation::WEST << 8);
            generateQuadMesh(mesh,
                    { {x+7.f/16.f, y+1, z+1},   {tex.right.x+tex.right.w, tex.right.y+tex.right.h}, data },
                    { {x+7.f/16.f, y+1, z},     {tex.right.x, tex.right.y+tex.right.h}, data },
                    { {x+7.f/16.f, y,   z},     {tex.right.x, tex.right.y}, data },
                    { {x+7.f/16.f, y,   z+1},   {tex.right.x+tex.right.w, tex.right.y}, data }
            );
            DebugStats::triCount += 1;
        }
        
        if(adj.top) {
            unsigned int data = light | (BlockOrientation::UP << 8);
            generateQuadMesh(mesh,
                    { {x+9.f/16.f, y+10.f/16.f, z+7.f/16.f},   {tex.top.x+tex.top.w * 9.f/16.f, tex.top.y + tex.top.w * 8.f/16.f}, data },
                    { {x+7.f/16.f,   y+10.f/16.f, z+7.f/16.f}, {tex.top.x + tex.top.w * 7.f/16.f, tex.top.y + tex.top.w * 8.f/16.f}, data },
                    { {x+7.f/16.f,   y+10.f/16.f, z+9.f/16.f}, {tex.top.x + tex.top.w * 7.f/16.f, tex.top.y+tex.top.h * 10.f/16.f}, data },
                    { {x+9.f/16.f, y+10.f/16.f, z+9.f/16.f}, {tex.top.x+tex.top.w * 9.f/16.f, tex.top.y+tex.top.h * 10.f/16.f}, data }
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
    
    void generateCrossMesh(Mesh<Vertex> &mesh, float x, float y, float z, BlockTexture tex, SurroundingBlocks adj, LightData light) {
        const static float off = glm::sqrt(2.f) / 4.f;
        // const static float off = .5;
        
        unsigned int data = light;
        data |= BlockOrientation::NORTH << 8;
        generateQuadMesh(mesh,
            { {x+.5f-off,   y+1, z+.5f - off}, {tex.front.x, tex.front.y+tex.front.h}, data},
            { {x+.5f-off,   y,   z+.5f - off}, {tex.front.x, tex.front.y}, data},
            { {x+.5f+off, y,   z+.5f + off}, {tex.front.x+tex.front.w, tex.front.y}, data},
            { {x+.5f+off, y+1, z+.5f + off}, {tex.front.x+tex.front.w, tex.front.y+tex.front.h}, data}
        );
        DebugStats::triCount += 1;

        generateQuadMesh(mesh,
            { {x+.5f-off,   y+1, z+.5f + off}, {tex.front.x, tex.front.y+tex.front.h}, data},
            { {x+.5f-off,   y,   z+.5f + off}, {tex.front.x, tex.front.y}, data},
            { {x+.5f+off, y,   z+.5f - off}, {tex.front.x+tex.front.w, tex.front.y}, data},
            { {x+.5f+off, y+1, z+.5f - off}, {tex.front.x+tex.front.w, tex.front.y+tex.front.h}, data}
        );
        DebugStats::triCount += 1;
    }

    
    void flushRegularBatch() {
        regularShader->use();
        regularShader->setUniform1f(sunUniform, skyBrightness);
        regularShader->setUniformMat4f(viewUniform, camera->getView());
        regularShader->setUniformMat4f(projUniform, camera->getProjection());
        regularBatch.flush();
    }
    
    void flushTransparentBatch() {
        // if(transparentBatch.isEmpty())
        //     return;
        
        // glDisable(GL_DEPTH_TEST);
        // glDisable(GL_CULL_FACE);
        // transparentShader->use();
        transparentShader->use();
        transparentShader->setUniform1f(wSunUniform, skyBrightness);
        transparentShader->setUniform1f(waveUniform, wave);
        transparentShader->setUniformMat4f(wViewUniform, camera->getView());
        transparentShader->setUniformMat4f(wProjUniform, camera->getProjection());
        transparentBatch.flush();
        // glEnable(GL_DEPTH_TEST);
        // glEnable(GL_CULL_FACE);
    }

    void renderMesh(const Vertex *mesh, const size_t size, const GLuint *indices, const size_t idc_size) {
        if(!regularBatch.hasRoomFor(size)) {
            flushRegularBatch();
            if(!regularBatch.hasRoomFor(size)) {
                fprintf(stderr, "Mesh too big for batch\n");
                return;
            }
        }
        regularBatch.addVertices(mesh, size, indices, idc_size);
    }

    void renderTransparentMesh(const Vertex *mesh, const size_t size, const GLuint *indices, const size_t idc_size) {
        if(!transparentBatch.hasRoomFor(size))
            flushTransparentBatch();
        if(!regularBatch.isEmpty()) // Ensure we're rendering over opaque blocks
            flushRegularBatch();
        
        if(transparentBatch.hasRoomFor(size))
            transparentBatch.addVertices(mesh, size, indices, idc_size);
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
    
    double sum = 0.0;
    
    void update(double deltaTime) {
        sum += deltaTime;
        if(sum > 1) {
            frame = ( frame + 1 ) % 4;
            sum = 0;
        }
        wave += (float)deltaTime*1.5f;
        if(wave > 2*glm::pi<float>())
            wave = 0;
        
        if(MinecraftClone::Input::isKeyDown(GLFW_KEY_R))
            printf("%f\n",wave);
        
        // for(auto &c : World::chunks) {
        //     if(c.second.isDirty()) {
        //         c.second.recalculateBleedLighting();
        //         c.second.rebuildMesh();
        //     }
        // }
    }
}
