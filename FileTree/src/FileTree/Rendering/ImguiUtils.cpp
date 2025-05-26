#include "imgui.h"
#include <string>
#include <filesystem>

namespace ImGuiUtils {
    void ShowTooltipIfHovered(const std::string& text) {
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", text.c_str());
            ImGui::EndTooltip();
        }
    }
}