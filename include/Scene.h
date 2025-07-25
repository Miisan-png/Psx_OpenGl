#pragma once

#include "Renderer.h"
#include <vector>
#include <memory>

class Scene {
public:
    std::vector<RenderObject> objects;
    
    void AddObject(Model* model, Texture* texture = nullptr) {
        RenderObject obj;
        obj.model = model;
        obj.texture = texture;
        obj.useTexture = (texture != nullptr);
        objects.push_back(obj);
    }
    
    void AddObjectAt(Model* model, float x, float y, float z, Texture* texture = nullptr) {
        RenderObject obj;
        obj.model = model;
        obj.texture = texture;
        obj.useTexture = (texture != nullptr);
        obj.transform.position[0] = x;
        obj.transform.position[1] = y;
        obj.transform.position[2] = z;
        objects.push_back(obj);
    }
    
    void Render(PSXRenderer& renderer) {
        for (const auto& obj : objects) {
            renderer.RenderObject(obj);
        }
    }
    
    void Clear() {
        objects.clear();
    }
};