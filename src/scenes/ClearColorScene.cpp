#include "scenes/ClearColorScene.h"
#include "core.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "input/input.h"

using namespace MinecraftClone;

ClearColorScene::ClearColorScene(MinecraftClone::Window *window) : clearColor{0.1f, 0.3f, 0.8f, 1.0f} {
    Input::enableCursor();
}

void ClearColorScene::render() {
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);
}

void ClearColorScene::guiRender() {
    ImGui::ColorEdit4("Clear Color", clearColor);
}
