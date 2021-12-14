#include "scene.h"

class ClearColorScene : public Scene {
    public:
        ClearColorScene();
        ~ClearColorScene();
        
        void update(float deltaTime) override;
        void render() override;
        void guiRender() override;
    private:
        float clearColor[4];
};
