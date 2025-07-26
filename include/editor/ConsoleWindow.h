#pragma once

#include <imgui.h>
#include <vector>
#include <string>

class ConsoleWindow {
public:
    bool isOpen;
    char inputBuffer[256];
    std::vector<std::string> items;
    std::vector<std::string> history;
    int historyPos;
    ImGuiTextFilter filter;
    bool autoScroll;
    bool scrollToBottom;
    int maxItems = 1000;

    ConsoleWindow();
    void Draw();
    void AddLog(const std::string& text);
    void ClearLog();
    void ExecCommand(const char* command_line);
    
    void Toggle() { isOpen = !isOpen; }
    bool IsOpen() const { return isOpen; }

private:
    static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);
    int TextEditCallback(ImGuiInputTextCallbackData* data);
};