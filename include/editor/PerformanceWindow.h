#pragma once

class PerformanceWindow {
public:
    static const int HISTORY_SIZE = 100;
    
    bool isOpen;
    float fpsHistory[HISTORY_SIZE];
    float frameTimeHistory[HISTORY_SIZE];
    int historyOffset;
    float frameTime;
    float fps;
    int frameCount;
    float fpsTimer;
    float maxFps;
    float maxFrameTime;

    PerformanceWindow();
    void Update(float deltaTime);
    void Draw();
    
    void Toggle() { isOpen = !isOpen; }
    bool IsOpen() const { return isOpen; }
};