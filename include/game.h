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
        
    }
    
    void Render() {
        renderer.BeginFrame(camera);
        scene.Render(renderer);
        renderer.EndFrame();
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
    }
};