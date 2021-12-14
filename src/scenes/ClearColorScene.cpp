#include "ClearColorScene.h"
#include "core.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

ClearColorScene::ClearColorScene() : clearColor{0.1f, 0.3f, 0.8f, 1.0f} {
    
}

ClearColorScene::~ClearColorScene() {
    
}

void ClearColorScene::update(float deltaTime) {
    
}

void ClearColorScene::render() {
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);
}

void ClearColorScene::guiRender() {
    // ImGui_ImplOpenGL3_NewFrame();
    // ImGui_ImplGlfw_NewFrame();
    // ImGui::NewFrame();
    
    // {
    //     ImGui::Begin("Change Clear Color");
    ImGui::ColorEdit4("Clear Color", clearColor);
        // ImGui::End();
    // }
    
    // ImGui::Render();
    // ImGui::EndFrame();
    // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
