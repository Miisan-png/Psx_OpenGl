#pragma once

#include <GLFW/glfw3.h>

// Forward declaration to avoid circular dependency
class Game;

class DebugUI {
public:
    bool showDebugWindow = false;
    bool showFogControls = false;
    bool showLightingControls = false;
    bool showParticleControls = false;
    bool showPostProcessControls = false;
    bool showPerformanceWindow = false;
    
    float frameTime = 0.0f;
    float fps = 0.0f;
    int frameCount = 0;
    float fpsTimer = 0.0f;
    
    bool Initialize(GLFWwindow* window);
    void Update(float deltaTime, Game& game);
    void Render();
    void ToggleDebugWindow();
    void TogglePerformanceWindow();
    void Shutdown();

private:
    void RenderDebugWindow(Game& game);
    void RenderPerformanceWindow();
};