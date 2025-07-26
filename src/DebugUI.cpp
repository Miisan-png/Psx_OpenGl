#include <glad/glad.h>
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "DebugUI.h"
#include "Game.h"
#include "editor/ConsoleWindow.h"
#include "editor/PerformanceWindow.h"
#include "editor/SceneViewportWindow.h"
#include "editor/ObjectInspectorWindow.h"
#include "editor/OutlinerWindow.h"
#include "editor/ImGuiTheme.h"

bool DebugUI::Initialize(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGuiTheme::SetPinkTheme();
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    consoleWindow = new ConsoleWindow();
    performanceWindow = new PerformanceWindow();
    sceneViewportWindow = new SceneViewportWindow();
    objectInspectorWindow = new ObjectInspectorWindow();
    outlinerWindow = new OutlinerWindow();
    
    sceneViewportWindow->SetInspectorWindow(objectInspectorWindow);
    outlinerWindow->SetInspectorWindow(objectInspectorWindow);
    
    consoleWindow->AddLog("[INFO] PSX Horror Engine initialized!");
    consoleWindow->AddLog("[INFO] Console system ready");
    
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
    
    performanceWindow->Update(deltaTime);
    
    if (showDebugWindow) {
        RenderDebugWindow(game);
    }
    
    consoleWindow->Draw();
    performanceWindow->Draw();
    sceneViewportWindow->Draw(game);
    objectInspectorWindow->Draw(game);
    outlinerWindow->Draw(game);
}

void DebugUI::Render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugUI::ToggleDebugWindow() {
    showDebugWindow = !showDebugWindow;
}

void DebugUI::TogglePerformanceWindow() {
    performanceWindow->Toggle();
}

void DebugUI::ToggleConsoleWindow() {
    consoleWindow->Toggle();
}

void DebugUI::ToggleSceneViewportWindow() {
    sceneViewportWindow->Toggle();
}

void DebugUI::ToggleObjectInspectorWindow() {
    objectInspectorWindow->Toggle();
}

void DebugUI::ToggleOutlinerWindow() {
    outlinerWindow->Toggle();
}

void DebugUI::Shutdown() {
    delete consoleWindow;
    delete performanceWindow;
    delete sceneViewportWindow;
    delete objectInspectorWindow;
    delete outlinerWindow;
    
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
    
    if (ImGui::CollapsingHeader("🌌 Apocalyptic Skybox")) {
        if (!game.renderer.skybox) {
            ImGui::Text("❌ Skybox not initialized!");
        } else {
            ImGui::Text("✅ Dark apocalyptic sky with neon meteors");
            ImGui::Text("⭐ Colorful twinkling stars active");
            ImGui::Text("☄️ Meteor trails: Cyan, Pink, Green");
            ImGui::Text("Animation Time: %.2f", game.renderer.skybox->totalTime);
            
            ImGui::Separator();
            ImGui::Text("🎮 This skybox is fully procedural!");
            ImGui::Text("🌟 Stars twinkle in different neon colors");
            ImGui::Text("💫 Meteors fly by every few seconds");
            ImGui::Text("🎨 PSX-style color quantization applied");
        }
    }
    
    if (ImGui::CollapsingHeader("Fog Controls")) {
        ImGui::SliderFloat("Fog Start", &game.renderer.fog.start, 0.1f, 10.0f);
        ImGui::SliderFloat("Fog End", &game.renderer.fog.end, 2.0f, 50.0f);
        ImGui::SliderFloat("Height Start", &game.renderer.fog.heightStart, -10.0f, 5.0f);
        ImGui::SliderFloat("Height End", &game.renderer.fog.heightEnd, -5.0f, 10.0f);
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
    
    if (ImGui::CollapsingHeader("Editor Windows")) {
        if (ImGui::Button("Toggle Console")) {
            ToggleConsoleWindow();
        }
        if (ImGui::Button("Toggle Performance")) {
            TogglePerformanceWindow();
        }
        if (ImGui::Button("Toggle Viewport")) {
            ToggleSceneViewportWindow();
        }
        if (ImGui::Button("Toggle Inspector")) {
            ToggleObjectInspectorWindow();
        }
        if (ImGui::Button("Toggle Outliner")) {
            ToggleOutlinerWindow();
        }
        
        ImGui::Separator();
        if (ImGui::Button("Pink Theme")) {
            ImGuiTheme::SetPinkTheme();
        }
        ImGui::SameLine();
        if (ImGui::Button("Red Theme")) {
            ImGuiTheme::SetRedTheme();
        }
        ImGui::SameLine();
        if (ImGui::Button("Dark Theme")) {
            ImGuiTheme::SetDarkTheme();
        }
        ImGui::SameLine();
        if (ImGui::Button("Light Theme")) {
            ImGuiTheme::SetLightTheme();
        }
    }
    
    ImGui::End();
}