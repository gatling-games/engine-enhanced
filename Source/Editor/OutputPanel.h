#pragma once

#include <imgui.h>

#include "Editor/EditorPanel.h"

class OutputPanel : public EditorPanel
{
public:
    // EditorPanel overrides
    virtual std::string name() const { return "Output Panel"; }
    virtual void draw();

    void log(const char* fmt, ...);

private:
    ImGuiTextBuffer     buffer_;
    ImGuiTextFilter     filter_;
    ImVector<int>       lineOffsets_;
    bool                scrollToBottom_;

    void clear();
};
