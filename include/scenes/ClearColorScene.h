#include "scenes/scene.h"
#include "core/window.h"

class ClearColorScene : public Scene {
    public:
        ClearColorScene(MinecraftClone::Window *window);
        
        void render(const Renderer &r) override;
        void guiRender() override;
    private:
        float clearColor[4];
};
