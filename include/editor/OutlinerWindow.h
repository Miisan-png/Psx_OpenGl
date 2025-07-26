#pragma once

#include "Game.h"

class ObjectInspectorWindow;

class OutlinerWindow {
public:
    bool isOpen;
    ObjectInspectorWindow* inspectorWindow;

    OutlinerWindow();
    void Draw(Game& game);
    void SetInspectorWindow(ObjectInspectorWindow* inspector);
    
    void Toggle() { isOpen = !isOpen; }
    bool IsOpen() const { return isOpen; }
};