#pragma once

#include <GLFW/glfw3.h>
#include <iostream>

class Game;
class ConsoleWindow;
class PerformanceWindow;
class SceneViewportWindow;

class DebugUI {
public:
    bool showDebugWindow = false;
    bool showSceneEditor = false;
    bool showAssetBrowser = false;
    bool isDragging = false;
    int dragAxis = -1;
    float dragStartValue = 0.0f;
    double mouseStartX = 0.0, mouseStartY = 0.0;
    
    float frameTime = 0.0f;
    float fps = 0.0f;
    int frameCount = 0;
    float fpsTimer = 0.0f;
    
    ConsoleWindow* consoleWindow = nullptr;
    PerformanceWindow* performanceWindow = nullptr;
    SceneViewportWindow* sceneViewportWindow = nullptr;
    
    bool Initialize(GLFWwindow* window);
    void Update(float deltaTime, Game& game);
    void Render();
    void ToggleDebugWindow();
    void TogglePerformanceWindow();
    void ToggleConsoleWindow();
    void ToggleSceneViewportWindow();
    void Shutdown();

    void ToggleSceneEditor() {
        showSceneEditor = !showSceneEditor;
    }

private:
    void RenderDebugWindow(Game& game);
};