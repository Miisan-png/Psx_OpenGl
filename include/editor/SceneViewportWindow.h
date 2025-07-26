#pragma once

#include "Camera.h"
#include "Shader.h"
#include "Game.h"
#include <vector>

class ObjectInspectorWindow;

enum class GizmoMode {
    TRANSLATE,
    ROTATE,
    SCALE
};

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
    
    GizmoMode currentGizmoMode;
    ObjectInspectorWindow* inspectorWindow;
    int hoveredObjectIndex;

    SceneViewportWindow();
    ~SceneViewportWindow();
    
    void Draw(Game& game);
    void HandleInput(Game& game);
    void RenderViewport(Game& game);
    void RenderGrid(const float* view, const float* projection);
    void RenderSceneObjects(Game& game, const float* view, const float* projection);
    void RenderGizmos(Game& game, const float* view, const float* projection);
    void HandleObjectSelection(Game& game);
    
    void SetInspectorWindow(ObjectInspectorWindow* inspector);
    
    void Toggle() { isOpen = !isOpen; }
    bool IsOpen() const { return isOpen; }

private:
    void CreateGridShader();
    void CreateGridGeometry();
    void CreateFramebuffer(int width, int height);
    void ResizeFramebuffer(int width, int height);
    void perspective(float fovy, float aspect, float zNear, float zFar, float* result);
    
    bool RayIntersectsBox(const float* rayOrigin, const float* rayDir, const RenderObject& obj, float& distance);
    void ScreenToWorldRay(float screenX, float screenY, float* rayOrigin, float* rayDir, const float* view, const float* projection);
};