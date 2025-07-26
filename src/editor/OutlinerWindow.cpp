#include "editor/OutlinerWindow.h"
#include "editor/ObjectInspectorWindow.h"
#include <imgui.h>

OutlinerWindow::OutlinerWindow() : isOpen(false), inspectorWindow(nullptr) {
}

void OutlinerWindow::Draw(Game& game) {
    if (!isOpen) return;
    
    ImGui::SetNextWindowSize(ImVec2(250, 400), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Scene Outliner", &isOpen)) {
        ImGui::End();
        return;
    }
    
    if (ImGui::Button("Add Object")) {
        ImGui::OpenPopup("AddObjectPopup");
    }
    
    if (ImGui::BeginPopup("AddObjectPopup")) {
        if (ImGui::MenuItem("Add Bed")) {
            if (game.bedModel.vertices.size() > 0) {
                game.scene.AddObjectAt(&game.bedModel, 0.0f, 0.0f, 0.0f, &game.bedTexture);
            }
        }
        if (ImGui::MenuItem("Add Empty Object")) {
            game.scene.AddObjectAt(nullptr, 0.0f, 0.0f, 0.0f, nullptr);
        }
        ImGui::EndPopup();
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Clear Scene")) {
        game.scene.Clear();
        if (inspectorWindow) {
            inspectorWindow->SetSelectedObject(-1);
        }
    }
    
    ImGui::Separator();
    ImGui::Text("Scene Objects (%zu):", game.scene.objects.size());
    
    if (game.scene.objects.empty()) {
        ImGui::Text("No objects in scene");
        ImGui::Text("Use 'Add Object' to create objects");
    } else {
        for (int i = 0; i < game.scene.objects.size(); i++) {
            const RenderObject& obj = game.scene.objects[i];
            
            bool isSelected = (inspectorWindow && inspectorWindow->GetSelectedObject() == i);
            
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            if (isSelected) {
                flags |= ImGuiTreeNodeFlags_Selected;
            }
            
            std::string objectName = "Object_" + std::to_string(i);
            if (obj.model && obj.model == &game.bedModel) {
                objectName = "Bed_" + std::to_string(i);
            } else if (!obj.model) {
                objectName = "Empty_" + std::to_string(i);
            }
            
            ImGui::TreeNodeEx((void*)(intptr_t)i, flags, "%s", objectName.c_str());
            
            if (ImGui::IsItemClicked()) {
                if (inspectorWindow) {
                    inspectorWindow->SetSelectedObject(i);
                }
            }
            
            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Select")) {
                    if (inspectorWindow) {
                        inspectorWindow->SetSelectedObject(i);
                    }
                }
                if (ImGui::MenuItem("Duplicate")) {
                    RenderObject duplicate = obj;
                    duplicate.transform.position[0] += 2.0f;
                    game.scene.objects.push_back(duplicate);
                }
                if (ImGui::MenuItem("Delete")) {
                    game.scene.objects.erase(game.scene.objects.begin() + i);
                    if (inspectorWindow && inspectorWindow->GetSelectedObject() >= i) {
                        int newSelection = inspectorWindow->GetSelectedObject() - 1;
                        if (newSelection >= game.scene.objects.size()) {
                            newSelection = -1;
                        }
                        inspectorWindow->SetSelectedObject(newSelection);
                    }
                    break;
                }
                ImGui::EndPopup();
            }
            
            ImGui::SameLine(200);
            ImGui::Text("(%.1f, %.1f, %.1f)", 
                obj.transform.position[0], 
                obj.transform.position[1], 
                obj.transform.position[2]);
        }
    }
    
    ImGui::End();
}

void OutlinerWindow::SetInspectorWindow(ObjectInspectorWindow* inspector) {
    inspectorWindow = inspector;
}