#include <glad/glad.h>
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "DebugUI.h"
#include "Game.h"

bool DebugUI::Initialize(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    return true;
}

void DebugUI::Update(float deltaTime, Game& game) {
    frameTime = deltaTime;
    frameCount++;
    fpsTimer += deltaTime;
    
    if (fpsTimer >= 1.0f) {
        fps = frameCount / fpsTimer;
        frameCount = 0;
        fpsTimer = 0.0f;
    }
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    if (showDebugWindow) {
        RenderDebugWindow(game);
    }
    
    if (showPerformanceWindow) {
        RenderPerformanceWindow();
    }
}

void DebugUI::Render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugUI::ToggleDebugWindow() {
    showDebugWindow = !showDebugWindow;
}

void DebugUI::TogglePerformanceWindow() {
    showPerformanceWindow = !showPerformanceWindow;
}

void DebugUI::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void DebugUI::RenderDebugWindow(Game& game) {
    ImGui::Begin("PSX Horror Engine Debug", &showDebugWindow);
    
    if (ImGui::CollapsingHeader("Scene Info")) {
        ImGui::Text("Objects in scene: %d", (int)game.scene.objects.size());
        ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", 
            game.camera.Position[0], game.camera.Position[1], game.camera.Position[2]);
        ImGui::Text("Camera Front: (%.2f, %.2f, %.2f)", 
            game.camera.Front[0], game.camera.Front[1], game.camera.Front[2]);
    }
    
    if (ImGui::CollapsingHeader("Fog Controls")) {
        ImGui::SliderFloat("Fog Start", &game.renderer.fog.start, 0.1f, 10.0f);
        ImGui::SliderFloat("Fog End", &game.renderer.fog.end, 2.0f, 50.0f);
        ImGui::ColorEdit3("Fog Color", game.renderer.fog.color);
    }
    
    if (ImGui::CollapsingHeader("Lighting Controls")) {
        ImGui::Checkbox("Spotlight Enabled", &game.renderer.lighting.spotlight.enabled);
        ImGui::SliderFloat("Spotlight Intensity", &game.renderer.lighting.spotlight.intensity, 0.0f, 5.0f);
        ImGui::SliderFloat("Spotlight Range", &game.renderer.lighting.spotlight.range, 1.0f, 30.0f);
        ImGui::SliderFloat("Inner Cone", &game.renderer.lighting.spotlight.innerCone, 5.0f, 45.0f);
        ImGui::SliderFloat("Outer Cone", &game.renderer.lighting.spotlight.outerCone, 10.0f, 60.0f);
        ImGui::ColorEdit3("Spotlight Color", game.renderer.lighting.spotlight.color);
        
        ImGui::Separator();
        ImGui::SliderFloat("Ambient Intensity", &game.renderer.lighting.ambient.intensity, 0.0f, 1.0f);
        ImGui::ColorEdit3("Ambient Color", game.renderer.lighting.ambient.color);
    }
    
    if (ImGui::CollapsingHeader("Particle System")) {
        ImGui::SliderFloat("Spawn Rate", &game.renderer.particles->spawnRate, 1.0f, 200.0f);
        
        int activeParticles = 0;
        for (const auto& p : game.renderer.particles->particles) {
            if (p.active) activeParticles++;
        }
        ImGui::Text("Active Particles: %d / %d", activeParticles, (int)game.renderer.particles->particles.size());
    }
    
    if (ImGui::CollapsingHeader("Post Processing")) {
        ImGui::Checkbox("Effects Enabled", &game.renderer.postProcess->effectsEnabled);
        
        const char* shaderNames[] = { "psx_retro", "scanlines", "crt_monitor" };
        static int currentShader = 0;
        
        if (game.renderer.postProcess->currentShaderName == "psx_retro") currentShader = 0;
        else if (game.renderer.postProcess->currentShaderName == "scanlines") currentShader = 1;
        else if (game.renderer.postProcess->currentShaderName == "crt_monitor") currentShader = 2;
        
        if (ImGui::Combo("Shader", &currentShader, shaderNames, 3)) {
            game.renderer.postProcess->currentShaderName = shaderNames[currentShader];
        }
        
        ImGui::SliderFloat("Vertex Snap Resolution", &game.renderer.vertexSnapResolution, 16.0f, 128.0f);
    }
    
    if (ImGui::CollapsingHeader("Render Settings")) {
        ImGui::Text("Render Resolution: %dx%d", game.renderer.renderWidth, game.renderer.renderHeight);
        ImGui::Text("Aspect Ratio: %.3f", game.renderer.currentAspectRatio);
    }
    
    ImGui::End();
}

void DebugUI::RenderPerformanceWindow() {
    ImGui::Begin("Performance", &showPerformanceWindow);
    
    ImGui::Text("FPS: %.1f", fps);
    ImGui::Text("Frame Time: %.3f ms", frameTime * 1000.0f);
    
    static float fpsHistory[100] = {};
    static int fpsHistoryOffset = 0;
    fpsHistory[fpsHistoryOffset] = fps;
    fpsHistoryOffset = (fpsHistoryOffset + 1) % 100;
    
    ImGui::PlotLines("FPS", fpsHistory, 100, fpsHistoryOffset, nullptr, 0.0f, 120.0f, ImVec2(0, 80));
    
    ImGui::Separator();
    ImGui::Text("Controls:");
    ImGui::Text("WASD - Move");
    ImGui::Text("F - Toggle Flashlight");
    ImGui::Text("0 - Toggle All Effects");
    ImGui::Text("1-3 - Switch Shaders");
    ImGui::Text("F11 - Fullscreen");
    ImGui::Text("F1 - Toggle Debug (this window)");
    ImGui::Text("F2 - Toggle Performance");
    
    ImGui::End();
}