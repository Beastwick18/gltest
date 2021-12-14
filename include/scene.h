#ifndef MINECRAFT_CLONE_SCENE_H
#define MINECRAFT_CLONE_SCENE_H

class Scene {
    public:
        Scene() {}
        virtual ~Scene() {}
        
        virtual void update(float deltaTime) {}
        virtual void render() {}
        virtual void guiRender() {}
};

#endif
