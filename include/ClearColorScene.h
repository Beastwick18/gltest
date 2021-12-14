#include "scene.h"

class ClearColorScene : public Scene {
    public:
        ClearColorScene(MinecraftClone::Window *window);
        ~ClearColorScene();
        
        void update() override;
        void render(const Renderer &r) override;
        void guiRender() override;
    private:
        float clearColor[4];
};
