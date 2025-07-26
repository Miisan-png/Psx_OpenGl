#pragma once

#include <iostream>
#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "DebugUI.h"
#include "PlayerController.h" // Add this include

class Game {
public:
    PSXRenderer renderer;
    Scene scene;
    Camera camera;
    DebugUI debugUI;
    PlayerController* playerController; // Add player controller
    
    Model bedModel;
    Texture bedTexture;
    
    bool Initialize(GLFWwindow* window) {
        camera = Camera(0.0f, 1.7f, 3.0f); // Set eye height to 1.7m (typical player height)

        if (!renderer.Initialize()) {
            return false;
        }
        
        if (!debugUI.Initialize(window)) {
            return false;
        }
        
        // Initialize player controller
        playerController = new PlayerController(&camera);
        
        if (bedModel.LoadFromFile("assets/GLB/bed.glb")) {
            if (bedTexture.LoadFromFile("assets/Texture/bed/Bed.png")) {
                LoadTestScene();
            }
        }
        
        return true;
    }
    
    void LoadTestScene() {
        scene.Clear();
        
        float positions[][3] = {
            {0.0f, 0.0f, 0.0f},
            {3.0f, 0.0f, -2.0f},
            {-3.0f, 0.0f, -4.0f},
            {0.0f, 0.0f, -6.0f},
            {5.0f, 0.0f, -8.0f},
            {-5.0f, 0.0f, -10.0f}
        };
        
        for (int i = 0; i < 6; i++) {
            scene.AddObjectAt(&bedModel, positions[i][0], positions[i][1], positions[i][2], &bedTexture);
        }
    }
    
    void Update(float deltaTime) {
        renderer.Update(deltaTime, camera);
        debugUI.Update(deltaTime, *this);
    }
    
    void Render(int screenWidth, int screenHeight) {
        renderer.BeginFrame(camera);
        scene.Render(renderer);
        renderer.EndFrame(camera, screenWidth, screenHeight);
        debugUI.Render();
    }
    
    void ProcessInput(GLFWwindow* window, float deltaTime) {
        // Use PlayerController for movement instead of direct camera control
        if (playerController) {
            playerController->ProcessKeyboardInput(window, deltaTime);
        }
        
        // Debug UI hotkeys work in both modes
        static bool fPressed = false;
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !fPressed) {
            renderer.lighting.ToggleSpotlight();
            fPressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
            fPressed = false;
        }
        
        static bool key1Pressed = false;
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !key1Pressed) {
            renderer.postProcess->currentShaderName = "psx_retro";
            std::cout << "Switched to PSX Retro effect" << std::endl;
            key1Pressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
            key1Pressed = false;
        }
        
        static bool key2Pressed = false;
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !key2Pressed) {
            renderer.postProcess->currentShaderName = "scanlines";
            std::cout << "Switched to Scanlines effect" << std::endl;
            key2Pressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
            key2Pressed = false;
        }
        
        static bool key3Pressed = false;
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !key3Pressed) {
            renderer.postProcess->currentShaderName = "crt_monitor";
            std::cout << "Switched to CRT Monitor effect" << std::endl;
            key3Pressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) {
            key3Pressed = false;
        }
        
        static bool key0Pressed = false;
        if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS && !key0Pressed) {
            renderer.postProcess->ToggleAllEffects();
            if (renderer.postProcess->effectsEnabled) {
                std::cout << "Post-processing effects ENABLED" << std::endl;
            } else {
                std::cout << "Post-processing effects DISABLED (raw render)" << std::endl;
            }
            key0Pressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_0) == GLFW_RELEASE) {
            key0Pressed = false;
        }
        
        static bool f1Pressed = false;
        if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS && !f1Pressed) {
            debugUI.ToggleDebugWindow();
            f1Pressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_RELEASE) {
            f1Pressed = false;
        }
        
        static bool f2Pressed = false;
        if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS && !f2Pressed) {
            debugUI.TogglePerformanceWindow();
            f2Pressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_RELEASE) {
            f2Pressed = false;
        }

        // Simple skybox info hotkey
        static bool f5Pressed = false;
        if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS && !f5Pressed) {
            std::cout << "🌌 Apocalyptic Skybox Active!" << std::endl;
            std::cout << "⭐ Colorful twinkling stars" << std::endl;
            std::cout << "☄️ Neon meteors with trails" << std::endl;
            f5Pressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_RELEASE) {
            f5Pressed = false;
        }
        
        // Head bob toggle for testing
        static bool hPressed = false;
        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS && !hPressed) {
            if (playerController) {
                playerController->SetHeadBobEnabled(!playerController->headBobEnabled);
                std::cout << "Head bob: " << (playerController->headBobEnabled ? "ENABLED" : "DISABLED") << std::endl;
            }
            hPressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE) {
            hPressed = false;
        }
    }
    
    // Add method for mouse input (called from main.cpp)
    void ProcessMouseMovement(float xOffset, float yOffset) {
        if (playerController) {
            playerController->ProcessMouseMovement(xOffset, yOffset);
        }
    }
    
    void Shutdown() {
        delete playerController; // Clean up player controller
        debugUI.Shutdown();
    }
};