#include "editor/PerformanceWindow.h"
#include <imgui.h>

PerformanceWindow::PerformanceWindow() : isOpen(false) {
    for (int i = 0; i < HISTORY_SIZE; i++) {
        fpsHistory[i] = 0.0f;
        frameTimeHistory[i] = 0.0f;
    }
    historyOffset = 0;
    frameTime = 0.0f;
    fps = 0.0f;
    frameCount = 0;
    fpsTimer = 0.0f;
    maxFps = 120.0f;
    maxFrameTime = 33.0f;
}

void PerformanceWindow::Update(float deltaTime) {
    frameTime = deltaTime;
    frameCount++;
    fpsTimer += deltaTime;
    
    if (fpsTimer >= 1.0f) {
        fps = frameCount / fpsTimer;
        frameCount = 0;
        fpsTimer = 0.0f;
        
        fpsHistory[historyOffset] = fps;
        frameTimeHistory[historyOffset] = frameTime * 1000.0f;
        historyOffset = (historyOffset + 1) % HISTORY_SIZE;
    }
}

void PerformanceWindow::Draw() {
    if (!isOpen) return;
    
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Performance Monitor", &isOpen)) {
        ImGui::End();
        return;
    }
    
    ImGui::Text("FPS: %.1f", fps);
    ImGui::SameLine(150);
    ImGui::Text("Frame Time: %.3f ms", frameTime * 1000.0f);
    
    ImGui::Separator();
    
    ImGui::Text("FPS Graph");
    ImGui::PlotLines("##fps", fpsHistory, HISTORY_SIZE, historyOffset, nullptr, 0.0f, maxFps, ImVec2(0, 80));
    
    ImGui::Text("Frame Time Graph (ms)");
    ImGui::PlotLines("##frametime", frameTimeHistory, HISTORY_SIZE, historyOffset, nullptr, 0.0f, maxFrameTime, ImVec2(0, 80));
    
    ImGui::Separator();
    
    if (ImGui::CollapsingHeader("Settings")) {
        ImGui::SliderFloat("Max FPS Scale", &maxFps, 60.0f, 240.0f);
        ImGui::SliderFloat("Max Frame Time Scale (ms)", &maxFrameTime, 16.0f, 100.0f);
        
        if (ImGui::Button("Reset Graphs")) {
            for (int i = 0; i < HISTORY_SIZE; i++) {
                fpsHistory[i] = 0.0f;
                frameTimeHistory[i] = 0.0f;
            }
            historyOffset = 0;
        }
    }
    
    if (ImGui::CollapsingHeader("Controls Reference")) {
        ImGui::Text("Movement:");
        ImGui::BulletText("WASD - Move camera");
        ImGui::BulletText("Mouse - Look around");
        ImGui::BulletText("TAB - Toggle UI mode");
        
        ImGui::Text("Lighting:");
        ImGui::BulletText("F - Toggle flashlight");
        
        ImGui::Text("Post Processing:");
        ImGui::BulletText("0 - Toggle all effects");
        ImGui::BulletText("1 - PSX Retro shader");
        ImGui::BulletText("2 - Scanlines shader");
        ImGui::BulletText("3 - CRT Monitor shader");
        
        ImGui::Text("Debug:");
        ImGui::BulletText("F1 - Toggle debug window");
        ImGui::BulletText("F2 - Toggle performance window");
        ImGui::BulletText("F5 - Skybox info");
        ImGui::BulletText("F11 - Toggle fullscreen");
    }
    
    ImGui::End();
}