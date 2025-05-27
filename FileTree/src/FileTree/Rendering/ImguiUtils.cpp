#include "imgui.h"
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
namespace FTree{
namespace ImGuiUtils {
    void ShowTooltipIfHovered(const std::string& text) {
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", text.c_str());
            ImGui::EndTooltip();
        }
    }
    // TODO: replace this not good place
    std::string readFile(const std::filesystem::path& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
}
}