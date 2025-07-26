#include "editor/ConsoleWindow.h"
#include <imgui.h>

ConsoleWindow::ConsoleWindow() : isOpen(true) {
    inputBuffer[0] = '\0';
    autoScroll = true;
    scrollToBottom = false;
}

void ConsoleWindow::Draw() {
    if (!isOpen) return;
    
    ImGui::SetNextWindowSize(ImVec2(520, 300), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Console", &isOpen)) {
        ImGui::End();
        return;
    }
    
    if (ImGui::BeginPopup("Options")) {
        ImGui::Checkbox("Auto-scroll", &autoScroll);
        ImGui::EndPopup();
    }
    
    if (ImGui::Button("Options")) {
        ImGui::OpenPopup("Options");
    }
    ImGui::SameLine();
    bool clear = ImGui::Button("Clear");
    ImGui::SameLine();
    bool copy = ImGui::Button("Copy");
    ImGui::SameLine();
    filter.Draw("Filter", -100.0f);
    
    ImGui::Separator();
    
    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeight();
    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar)) {
        if (clear) {
            ClearLog();
        }
        if (copy) {
            ImGui::LogToClipboard();
        }
        
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
        for (int i = 0; i < items.size(); i++) {
            const char* item = items[i].c_str();
            if (!filter.PassFilter(item)) continue;
            
            ImVec4 color;
            bool hasColor = false;
            if (strstr(item, "[ERROR]")) { 
                color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); 
                hasColor = true; 
            } else if (strstr(item, "[WARNING]")) { 
                color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); 
                hasColor = true; 
            } else if (strstr(item, "[INFO]")) { 
                color = ImVec4(0.4f, 0.8f, 1.0f, 1.0f); 
                hasColor = true; 
            }
            
            if (hasColor) ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(item);
            if (hasColor) ImGui::PopStyleColor();
        }
        
        if (scrollToBottom || (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())) {
            ImGui::SetScrollHereY(1.0f);
        }
        scrollToBottom = false;
        
        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
    ImGui::Separator();
    
    bool reclaim_focus = false;
    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
    if (ImGui::InputText("Input", inputBuffer, IM_ARRAYSIZE(inputBuffer), input_text_flags, &TextEditCallbackStub, (void*)this)) {
        char* s = inputBuffer;
        if (s[0]) ExecCommand(s);
        strcpy(s, "");
        reclaim_focus = true;
    }
    
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus) ImGui::SetKeyboardFocusHere(-1);
    
    ImGui::End();
}

void ConsoleWindow::AddLog(const std::string& text) {
    items.push_back(text);
    if (items.size() > maxItems) {
        items.erase(items.begin());
    }
    scrollToBottom = true;
}

void ConsoleWindow::ClearLog() {
    items.clear();
}

void ConsoleWindow::ExecCommand(const char* command_line) {
    AddLog(std::string("> ") + command_line);
    
    historyPos = -1;
    for (int i = history.size() - 1; i >= 0; i--) {
        if (strcmp(history[i].c_str(), command_line) == 0) {
            history.erase(history.begin() + i);
            break;
        }
    }
    history.push_back(std::string(command_line));
    
    if (strcmp(command_line, "clear") == 0) {
        ClearLog();
    } else if (strcmp(command_line, "help") == 0) {
        AddLog("[INFO] Available commands:");
        AddLog("  clear - Clear console");
        AddLog("  help - Show this help");
        AddLog("  history - Show command history");
    } else if (strcmp(command_line, "history") == 0) {
        int first = history.size() - 10;
        for (int i = first > 0 ? first : 0; i < history.size(); i++) {
            AddLog(std::string("  ") + std::to_string(i) + ": " + history[i]);
        }
    } else {
        AddLog(std::string("[ERROR] Unknown command: '") + command_line + "'");
    }
}

int ConsoleWindow::TextEditCallbackStub(ImGuiInputTextCallbackData* data) {
    ConsoleWindow* console = (ConsoleWindow*)data->UserData;
    return console->TextEditCallback(data);
}

int ConsoleWindow::TextEditCallback(ImGuiInputTextCallbackData* data) {
    switch (data->EventFlag) {
        case ImGuiInputTextFlags_CallbackCompletion: {
            const char* word_end = data->Buf + data->CursorPos;
            const char* word_start = word_end;
            while (word_start > data->Buf) {
                const char c = word_start[-1];
                if (c == ' ' || c == '\t' || c == ',' || c == ';') break;
                word_start--;
            }
            
            std::vector<std::string> candidates;
            candidates.push_back("clear");
            candidates.push_back("help");
            candidates.push_back("history");
            
            std::vector<std::string> matches;
            for (auto& candidate : candidates) {
                if (strncmp(candidate.c_str(), word_start, (int)(word_end - word_start)) == 0) {
                    matches.push_back(candidate);
                }
            }
            
            if (matches.size() == 0) {
                AddLog("[INFO] No match for \"" + std::string(word_start, word_end) + "\"");
            } else if (matches.size() == 1) {
                data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                data->InsertChars(data->CursorPos, matches[0].c_str());
                data->InsertChars(data->CursorPos, " ");
            } else {
                int match_len = (int)(word_end - word_start);
                for (;;) {
                    int c = 0;
                    bool all_candidates_matches = true;
                    for (int i = 0; i < matches.size() && all_candidates_matches; i++) {
                        if (i == 0) c = toupper(matches[i][match_len]);
                        else if (c == 0 || c != toupper(matches[i][match_len])) all_candidates_matches = false;
                    }
                    if (!all_candidates_matches) break;
                    match_len++;
                }
                
                if (match_len > 0) {
                    data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                    data->InsertChars(data->CursorPos, matches[0].c_str(), matches[0].c_str() + match_len);
                }
                
                AddLog("[INFO] Possible matches:");
                for (auto& match : matches) {
                    AddLog(std::string("  ") + match);
                }
            }
            break;
        }
        case ImGuiInputTextFlags_CallbackHistory: {
            const int prev_history_pos = historyPos;
            if (data->EventKey == ImGuiKey_UpArrow) {
                if (historyPos == -1) historyPos = history.size() - 1;
                else if (historyPos > 0) historyPos--;
            } else if (data->EventKey == ImGuiKey_DownArrow) {
                if (historyPos != -1) {
                    if (++historyPos >= history.size()) historyPos = -1;
                }
            }
            
            if (prev_history_pos != historyPos) {
                const char* history_str = (historyPos >= 0) ? history[historyPos].c_str() : "";
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, history_str);
            }
        }
    }
    return 0;
}