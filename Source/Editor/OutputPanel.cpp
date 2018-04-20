#include "OutputPanel.h"

void OutputPanel::draw()
{
    if (ImGui::Button("Clear")) clear();
    ImGui::SameLine();
    bool copy = ImGui::Button("Copy");
    ImGui::SameLine();
    filter_.Draw("Filter", -100.0f);
    ImGui::Separator();
    ImGui::BeginChild("LogOutput", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    if (copy) ImGui::LogToClipboard();
    if (filter_.IsActive())
    {
        const char* buf_begin = buffer_.begin();
        const char* line = buf_begin;
        for (int line_no = 0; line != NULL; line_no++)
        {
            const char* line_end = (line_no < lineOffsets_.Size) ? buf_begin + lineOffsets_[line_no] : NULL;
            if (filter_.PassFilter(line, line_end))
            {
                ImGui::TextUnformatted(line, line_end);
            }
            line = line_end && line_end[1] ? line_end + 1 : NULL;
        }
    }
    else
    {
        ImGui::TextUnformatted(buffer_.begin());
    }

    if (scrollToBottom_)
    {
        ImGui::SetScrollHere(1.0f);
    }
    scrollToBottom_ = false;
    ImGui::EndChild();
}

void OutputPanel::clear()
{
    buffer_.clear(); lineOffsets_.clear();
}

void OutputPanel::log(const char* fmt, va_list args)
{
    int old_size = buffer_.size();
    buffer_.appendv(fmt, args);
    for (int new_size = buffer_.size(); old_size < new_size; old_size++)
        if (buffer_[old_size] == '\n')
        {
            lineOffsets_.push_back(old_size);
        }
    scrollToBottom_ = true;
}
