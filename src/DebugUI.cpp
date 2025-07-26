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

    // Add this section to your existing RenderDebugWindow function in DebugUI.cpp
// Replace the existing "Lighting Controls" section with this enhanced version:

if (ImGui::CollapsingHeader("💡 Enhanced Lighting System", ImGuiTreeNodeFlags_DefaultOpen)) {
    // Directional Light Controls
    if (ImGui::TreeNode("🌞 Directional Light (General Scene Lighting)")) {
        ImGui::Checkbox("Enable Directional Light", &game.renderer.lighting.directional.enabled);
        
        if (game.renderer.lighting.directional.enabled) {
            ImGui::Separator();
            ImGui::Text("Direction (X, Y, Z):");
            if (ImGui::SliderFloat3("Direction", game.renderer.lighting.directional.direction, -1.0f, 1.0f)) {
                game.renderer.lighting.directional.NormalizeDirection();
            }
            
            ImGui::SliderFloat("Intensity", &game.renderer.lighting.directional.intensity, 0.0f, 2.0f);
            ImGui::ColorEdit3("Color", game.renderer.lighting.directional.color);
            
            ImGui::Separator();
            ImGui::Text("Quick Presets:");
            if (ImGui::Button("☀️ Sunlight")) {
                game.renderer.lighting.SetSunlightPreset();
            }
            ImGui::SameLine();
            if (ImGui::Button("🌙 Moonlight")) {
                game.renderer.lighting.SetMoonlightPreset();
            }
            ImGui::SameLine();
            if (ImGui::Button("👻 Horror")) {
                game.renderer.lighting.SetHorrorPreset();
            }
            
            // Real-time direction helpers
            ImGui::Separator();
            ImGui::Text("Direction Helpers:");
            if (ImGui::Button("⬇️ Top Down")) {
                game.renderer.lighting.SetDirectionalDirection(0.0f, -1.0f, 0.0f);
            }
            ImGui::SameLine();
            if (ImGui::Button("↗️ Angled")) {
                game.renderer.lighting.SetDirectionalDirection(-0.3f, -0.8f, -0.5f);
            }
            ImGui::SameLine();
            if (ImGui::Button("➡️ Side")) {
                game.renderer.lighting.SetDirectionalDirection(1.0f, 0.0f, 0.0f);
            }
        }
        ImGui::TreePop();
    }
    
    // Spotlight Controls (Enhanced)
    if (ImGui::TreeNode("🔦 Spotlight (Flashlight)")) {
        ImGui::Checkbox("Enable Spotlight", &game.renderer.lighting.spotlight.enabled);
        
        if (game.renderer.lighting.spotlight.enabled) {
            ImGui::SliderFloat("Intensity", &game.renderer.lighting.spotlight.intensity, 0.0f, 5.0f);
            ImGui::SliderFloat("Range", &game.renderer.lighting.spotlight.range, 1.0f, 30.0f);
            ImGui::SliderFloat("Inner Cone", &game.renderer.lighting.spotlight.innerCone, 5.0f, 45.0f);
            ImGui::SliderFloat("Outer Cone", &game.renderer.lighting.spotlight.outerCone, 10.0f, 60.0f);
            ImGui::ColorEdit3("Color", game.renderer.lighting.spotlight.color);
            
            ImGui::Separator();
            ImGui::Text("Quick Colors:");
            if (ImGui::Button("🟡 Warm")) {
                game.renderer.lighting.spotlight.color[0] = 1.0f;
                game.renderer.lighting.spotlight.color[1] = 0.9f;
                game.renderer.lighting.spotlight.color[2] = 0.7f;
            }
            ImGui::SameLine();
            if (ImGui::Button("⚪ Cool")) {
                game.renderer.lighting.spotlight.color[0] = 0.7f;
                game.renderer.lighting.spotlight.color[1] = 0.8f;
                game.renderer.lighting.spotlight.color[2] = 1.0f;
            }
            ImGui::SameLine();
            if (ImGui::Button("🔴 Red")) {
                game.renderer.lighting.spotlight.color[0] = 1.0f;
                game.renderer.lighting.spotlight.color[1] = 0.3f;
                game.renderer.lighting.spotlight.color[2] = 0.3f;
            }
        }
        ImGui::TreePop();
    }
    
    // Ambient Light Controls
    if (ImGui::TreeNode("🌫️ Ambient Light (Base Level)")) {
        ImGui::SliderFloat("Intensity", &game.renderer.lighting.ambient.intensity, 0.0f, 1.0f);
        ImGui::ColorEdit3("Color", game.renderer.lighting.ambient.color);
        
        ImGui::Separator();
        ImGui::Text("Ambient Presets:");
        if (ImGui::Button("🌅 Dawn")) {
            game.renderer.lighting.ambient.color[0] = 0.2f;
            game.renderer.lighting.ambient.color[1] = 0.15f;
            game.renderer.lighting.ambient.color[2] = 0.1f;
            game.renderer.lighting.ambient.intensity = 0.4f;
        }
        ImGui::SameLine();
        if (ImGui::Button("🌃 Night")) {
            game.renderer.lighting.ambient.color[0] = 0.05f;
            game.renderer.lighting.ambient.color[1] = 0.08f;
            game.renderer.lighting.ambient.color[2] = 0.15f;
            game.renderer.lighting.ambient.intensity = 0.2f;
        }
        ImGui::SameLine();
        if (ImGui::Button("👹 Horror")) {
            game.renderer.lighting.ambient.color[0] = 0.1f;
            game.renderer.lighting.ambient.color[1] = 0.05f;
            game.renderer.lighting.ambient.color[2] = 0.15f;
            game.renderer.lighting.ambient.intensity = 0.3f;
        }
        ImGui::TreePop();
    }
    
    // Lighting System Overview
    ImGui::Separator();
    if (ImGui::CollapsingHeader("📊 Lighting Overview")) {
        ImGui::Text("Active Lights:");
        ImGui::BulletText("Directional: %s (%.1f intensity)", 
            game.renderer.lighting.directional.enabled ? "ON" : "OFF",
            game.renderer.lighting.directional.intensity);
        ImGui::BulletText("Spotlight: %s (%.1f intensity)", 
            game.renderer.lighting.spotlight.enabled ? "ON" : "OFF",
            game.renderer.lighting.spotlight.intensity);
        ImGui::BulletText("Ambient: %.1f intensity", 
            game.renderer.lighting.ambient.intensity);
        
        ImGui::Separator();
        if (ImGui::Button("💡 All Lights ON")) {
            game.renderer.lighting.directional.enabled = true;
            game.renderer.lighting.spotlight.enabled = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("🌑 All Lights OFF")) {
            game.renderer.lighting.directional.enabled = false;
            game.renderer.lighting.spotlight.enabled = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("🔄 Reset All")) {
            game.renderer.lighting = LightingSystem(); // Reset to defaults
        }
    }
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