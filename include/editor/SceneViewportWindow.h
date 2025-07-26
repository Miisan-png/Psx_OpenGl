#pragma once

#include "Camera.h"
#include "Shader.h"
#include "Game.h"
#include <vector>

class SceneViewportWindow {
public:
    bool isOpen;
    Camera viewportCamera;
    
    bool showGrid;
    bool showGizmos;
    bool showWireframe;
    int gridSize;
    float gridSpacing;
    
    unsigned int framebuffer;
    unsigned int colorTexture;
    unsigned int depthTexture;
    int framebufferWidth;
    int framebufferHeight;
    
    unsigned int gridVAO, gridVBO, gridEBO;
    unsigned int gridIndexCount;
    Shader* gridShader;
    
    float lastMouseX, lastMouseY;
    bool isDragging;
    bool isMiddleMouseDown;

    SceneViewportWindow();
    ~SceneViewportWindow();
    
    void Draw(Game& game);
    void HandleInput();
    void RenderViewport(Game& game);
    void RenderGrid(const float* view, const float* projection);
    void RenderSceneObjects(Game& game, const float* view, const float* projection);
    
    void Toggle() { isOpen = !isOpen; }
    bool IsOpen() const { return isOpen; }

private:
    void CreateGridShader();
    void CreateGridGeometry();
    void CreateFramebuffer(int width, int height);
    void ResizeFramebuffer(int width, int height);
    void perspective(float fovy, float aspect, float zNear, float zFar, float* result);
};