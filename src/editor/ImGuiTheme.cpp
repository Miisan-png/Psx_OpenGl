#include "editor/ImGuiTheme.h"
#include <imgui.h>

void ImGuiTheme::SetRedTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    
    colors[ImGuiCol_Text]                   = ImVec4(0.98f, 0.90f, 0.88f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.60f, 0.40f, 0.35f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.05f, 0.05f, 0.95f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.15f, 0.08f, 0.08f, 0.20f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.18f, 0.08f, 0.08f, 0.96f);
    colors[ImGuiCol_Border]                 = ImVec4(0.90f, 0.25f, 0.20f, 0.70f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.40f, 0.10f, 0.05f, 0.35f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.35f, 0.15f, 0.12f, 0.65f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.85f, 0.35f, 0.25f, 0.55f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.95f, 0.45f, 0.35f, 0.85f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.20f, 0.08f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.80f, 0.20f, 0.15f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.40f, 0.15f, 0.12f, 0.65f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.28f, 0.10f, 0.08f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.22f, 0.08f, 0.06f, 0.65f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.75f, 0.25f, 0.20f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.90f, 0.35f, 0.30f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(1.00f, 0.45f, 0.40f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(1.00f, 0.50f, 0.40f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.90f, 0.30f, 0.25f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(1.00f, 0.50f, 0.45f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.75f, 0.20f, 0.15f, 0.55f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.90f, 0.35f, 0.30f, 0.90f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(1.00f, 0.50f, 0.45f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.75f, 0.20f, 0.15f, 0.45f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.90f, 0.35f, 0.30f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(1.00f, 0.45f, 0.40f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.65f, 0.20f, 0.15f, 0.65f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.85f, 0.30f, 0.25f, 0.90f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(1.00f, 0.45f, 0.40f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.75f, 0.20f, 0.15f, 0.35f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.90f, 0.35f, 0.30f, 0.80f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(1.00f, 0.50f, 0.45f, 1.00f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.50f, 0.15f, 0.12f, 0.92f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.90f, 0.35f, 0.30f, 0.85f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.80f, 0.25f, 0.20f, 1.00f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.35f, 0.12f, 0.10f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.60f, 0.18f, 0.15f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 0.45f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.65f, 0.55f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.35f, 0.25f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.50f, 0.40f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.40f, 0.12f, 0.10f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.65f, 0.20f, 0.15f, 1.00f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.45f, 0.15f, 0.12f, 1.00f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(0.90f, 0.30f, 0.20f, 0.10f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.90f, 0.35f, 0.25f, 0.50f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 0.60f, 0.50f, 1.00f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.95f, 0.40f, 0.30f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.70f, 0.60f, 0.85f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.70f, 0.20f, 0.15f, 0.30f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.50f, 0.15f, 0.10f, 0.50f);
    
    style.WindowPadding     = ImVec2(8.0f, 8.0f);
    style.FramePadding      = ImVec2(4.0f, 3.0f);
    style.CellPadding       = ImVec2(4.0f, 2.0f);
    style.ItemSpacing       = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing  = ImVec2(4.0f, 4.0f);
    style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
    style.IndentSpacing     = 21.0f;
    style.ScrollbarSize     = 14.0f;
    style.GrabMinSize       = 10.0f;
    
    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize  = 1.0f;
    style.PopupBorderSize  = 1.0f;
    style.FrameBorderSize  = 0.0f;
    style.TabBorderSize    = 0.0f;
    
    style.WindowRounding    = 5.0f;
    style.ChildRounding     = 3.0f;
    style.FrameRounding     = 3.0f;
    style.PopupRounding     = 3.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabRounding      = 3.0f;
    style.LogSliderDeadzone = 4.0f;
    style.TabRounding       = 4.0f;
}

void ImGuiTheme::SetPinkTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    
    colors[ImGuiCol_Text]                   = ImVec4(0.98f, 0.92f, 0.95f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.55f, 0.45f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.08f, 0.05f, 0.10f, 0.95f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.12f, 0.08f, 0.14f, 0.20f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.15f, 0.10f, 0.18f, 0.96f);
    colors[ImGuiCol_Border]                 = ImVec4(0.85f, 0.35f, 0.65f, 0.65f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.30f, 0.10f, 0.25f, 0.30f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.30f, 0.18f, 0.28f, 0.60f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.90f, 0.45f, 0.75f, 0.50f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.95f, 0.55f, 0.85f, 0.80f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.18f, 0.08f, 0.16f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.75f, 0.25f, 0.60f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.35f, 0.15f, 0.30f, 0.60f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.25f, 0.12f, 0.22f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.20f, 0.10f, 0.18f, 0.60f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.70f, 0.30f, 0.55f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.85f, 0.40f, 0.70f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.95f, 0.50f, 0.85f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(1.00f, 0.65f, 0.90f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.85f, 0.40f, 0.70f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.95f, 0.60f, 0.85f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.70f, 0.25f, 0.55f, 0.50f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.90f, 0.45f, 0.75f, 0.85f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.95f, 0.60f, 0.85f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.70f, 0.25f, 0.55f, 0.40f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.85f, 0.45f, 0.75f, 0.75f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.95f, 0.55f, 0.85f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.60f, 0.25f, 0.45f, 0.60f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.80f, 0.40f, 0.65f, 0.85f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.95f, 0.55f, 0.80f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.70f, 0.25f, 0.55f, 0.30f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.85f, 0.45f, 0.75f, 0.75f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.95f, 0.60f, 0.85f, 1.00f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.45f, 0.20f, 0.38f, 0.90f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.85f, 0.45f, 0.75f, 0.85f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.75f, 0.35f, 0.65f, 1.00f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.30f, 0.12f, 0.25f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.55f, 0.25f, 0.45f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.95f, 0.55f, 0.85f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.70f, 0.95f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.85f, 0.45f, 0.75f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(0.95f, 0.60f, 0.85f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.35f, 0.15f, 0.30f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.60f, 0.25f, 0.50f, 1.00f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.40f, 0.18f, 0.35f, 1.00f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(0.90f, 0.50f, 0.80f, 0.08f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.85f, 0.45f, 0.75f, 0.45f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 0.70f, 0.95f, 1.00f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.90f, 0.50f, 0.80f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.80f, 0.95f, 0.80f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.70f, 0.30f, 0.60f, 0.25f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.50f, 0.20f, 0.40f, 0.45f);
    
    style.WindowPadding     = ImVec2(8.0f, 8.0f);
    style.FramePadding      = ImVec2(4.0f, 3.0f);
    style.CellPadding       = ImVec2(4.0f, 2.0f);
    style.ItemSpacing       = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing  = ImVec2(4.0f, 4.0f);
    style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
    style.IndentSpacing     = 21.0f;
    style.ScrollbarSize     = 14.0f;
    style.GrabMinSize       = 10.0f;
    
    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize  = 1.0f;
    style.PopupBorderSize  = 1.0f;
    style.FrameBorderSize  = 0.0f;
    style.TabBorderSize    = 0.0f;
    
    style.WindowRounding    = 5.0f;
    style.ChildRounding     = 3.0f;
    style.FrameRounding     = 3.0f;
    style.PopupRounding     = 3.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabRounding      = 3.0f;
    style.LogSliderDeadzone = 4.0f;
    style.TabRounding       = 4.0f;
}

void ImGuiTheme::SetDarkTheme() {
    ImGui::StyleColorsDark();
}

void ImGuiTheme::SetLightTheme() {
    ImGui::StyleColorsLight();
}

void ImGuiTheme::SetClassicTheme() {
    ImGui::StyleColorsClassic();
}