#include "editor/ObjectInspectorWindow.h"
#include <imgui.h>

ObjectInspectorWindow::ObjectInspectorWindow() : isOpen(false), selectedObjectIndex(-1) {
}

void ObjectInspectorWindow::Draw(Game& game) {
    if (!isOpen) return;
    
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Object Inspector", &isOpen)) {
        ImGui::End();
        return;
    }
    
    if (selectedObjectIndex < 0 || selectedObjectIndex >= game.scene.objects.size()) {
        ImGui::Text("No object selected");
        ImGui::Text("Click on an object in the viewport or outliner to select it.");
        ImGui::End();
        return;
    }
    
    RenderObject& obj = game.scene.objects[selectedObjectIndex];
    
    ImGui::Text("Selected Object #%d", selectedObjectIndex);
    ImGui::Separator();
    
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Position:");
        ImGui::DragFloat3("##Position", obj.transform.position, 0.1f, -100.0f, 100.0f);
        
        ImGui::Text("Rotation:");
        float rotationDegrees[3] = {
            obj.transform.rotation[0] * 57.2958f,
            obj.transform.rotation[1] * 57.2958f,
            obj.transform.rotation[2] * 57.2958f
        };
        if (ImGui::DragFloat3("##Rotation", rotationDegrees, 1.0f, -180.0f, 180.0f)) {
            obj.transform.rotation[0] = rotationDegrees[0] * 0.0174533f;
            obj.transform.rotation[1] = rotationDegrees[1] * 0.0174533f;
            obj.transform.rotation[2] = rotationDegrees[2] * 0.0174533f;
        }
        
        ImGui::Text("Scale:");
        ImGui::DragFloat3("##Scale", obj.transform.scale, 0.01f, 0.1f, 10.0f);
        
        if (ImGui::Button("Reset Transform")) {
            obj.transform.position[0] = obj.transform.position[1] = obj.transform.position[2] = 0.0f;
            obj.transform.rotation[0] = obj.transform.rotation[1] = obj.transform.rotation[2] = 0.0f;
            obj.transform.scale[0] = obj.transform.scale[1] = obj.transform.scale[2] = 1.0f;
        }
    }
    
    if (ImGui::CollapsingHeader("Rendering")) {
        ImGui::Checkbox("Use Texture", &obj.useTexture);
        
        if (obj.model) {
            ImGui::Text("Model: Loaded (%zu vertices)", obj.model->vertices.size());
        } else {
            ImGui::Text("Model: None");
        }
        
        if (obj.texture) {
            ImGui::Text("Texture: %dx%d", obj.texture->width, obj.texture->height);
        } else {
            ImGui::Text("Texture: None");
        }
    }
    
    if (ImGui::CollapsingHeader("Actions")) {
        if (ImGui::Button("Duplicate Object")) {
            DuplicateObject(game, selectedObjectIndex);
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete Object")) {
            DeleteObject(game, selectedObjectIndex);
        }
        
        if (ImGui::Button("Focus in Viewport")) {
            FocusObjectInViewport(game);
        }
    }
    
    ImGui::End();
}

void ObjectInspectorWindow::SetSelectedObject(int index) {
    selectedObjectIndex = index;
}

int ObjectInspectorWindow::GetSelectedObject() const {
    return selectedObjectIndex;
}

void ObjectInspectorWindow::DuplicateObject(Game& game, int objectIndex) {
    if (objectIndex < 0 || objectIndex >= game.scene.objects.size()) return;
    
    RenderObject& original = game.scene.objects[objectIndex];
    RenderObject duplicate = original;
    
    duplicate.transform.position[0] += 2.0f;
    
    game.scene.objects.push_back(duplicate);
    selectedObjectIndex = game.scene.objects.size() - 1;
}

void ObjectInspectorWindow::DeleteObject(Game& game, int objectIndex) {
    if (objectIndex < 0 || objectIndex >= game.scene.objects.size()) return;
    
    game.scene.objects.erase(game.scene.objects.begin() + objectIndex);
    
    if (selectedObjectIndex >= objectIndex) {
        selectedObjectIndex--;
    }
    
    if (selectedObjectIndex >= game.scene.objects.size()) {
        selectedObjectIndex = -1;
    }
}

void ObjectInspectorWindow::FocusObjectInViewport(Game& game) {
    if (selectedObjectIndex < 0 || selectedObjectIndex >= game.scene.objects.size()) return;
    
}