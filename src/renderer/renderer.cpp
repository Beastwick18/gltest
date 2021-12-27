#include "renderer/renderer.h"

namespace Renderer {
    Shader *regularShader, *transparentShader;
    GLint vpUniform, wVpUniform;
    GLint waveUniform;
    Batch<Vertex> regularBatch;
    Batch<Vertex> transparentBatch;
    
    static const Camera *camera;
    
    float wave = 0;
    
    void init() {
        camera = nullptr;
        VBlayout layout;
        layout.push<float>(3);
        layout.push<float>(3);
        layout.push<float>(2);
        regularBatch.init(layout);
        transparentBatch.init(layout);
        
        regularShader = Shader::createShader("assets/shaders/batch3d.glsl");
        regularShader->use();
        regularShader->setUniform1i("tex0", 0);
        regularShader->setUniformMat4f("model", glm::mat4(1.f));
        vpUniform = regularShader->getUniformLocation("viewProj");
        
        transparentShader = Shader::createShader("assets/shaders/waveyBlock.glsl");
        transparentShader->use();
        transparentShader->setUniform1i("tex0", 0);
        transparentShader->setUniformMat4f("model", glm::mat4(1.f));
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
    
    void flushRegularBatch() {
        // if(regularBatch.isEmtpy())
        //     return;
        
        regularShader->use();
        regularShader->setUniformMat4f(vpUniform, camera->getProjection() * camera->getView());
        regularBatch.flush();
    }
    
    void flushTransparentBatch() {
        if(transparentBatch.isEmpty())
            return;
        
        // glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        transparentShader->use();
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
