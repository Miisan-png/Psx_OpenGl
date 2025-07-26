#pragma once

#include "Game.h"

class ObjectInspectorWindow {
public:
    bool isOpen;
    int selectedObjectIndex;

    ObjectInspectorWindow();
    void Draw(Game& game);
    void SetSelectedObject(int index);
    int GetSelectedObject() const;
    
    void Toggle() { isOpen = !isOpen; }
    bool IsOpen() const { return isOpen; }

private:
    void DuplicateObject(Game& game, int objectIndex);
    void DeleteObject(Game& game, int objectIndex);
    void FocusObjectInViewport(Game& game);
};