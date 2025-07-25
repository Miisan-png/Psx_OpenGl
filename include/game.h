#pragma once

#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"

class Game {
public:
    PSXRenderer renderer;
    Scene scene;
    Camera camera;
    
    Model bedModel;
    Texture bedTexture;
    
    bool Initialize() {
        camera = Camera(0.0f, 0.0f, 3.0f);
        
        if (!renderer.Initialize()) {
            return false;
        }
        
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
    }
    
    void Render(int screenWidth, int screenHeight) {
        renderer.BeginFrame(camera);
        scene.Render(renderer);
        renderer.EndFrame(camera, screenWidth, screenHeight);
    }
    
    void ProcessInput(GLFWwindow* window, float deltaTime) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(0, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(1, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(2, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(3, deltaTime);
        
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
    }
};