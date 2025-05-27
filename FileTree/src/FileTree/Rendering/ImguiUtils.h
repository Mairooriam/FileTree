#pragma once
#include "imgui.h"
#include <string>
#include <filesystem>
namespace FTree{
namespace ImGuiUtils {
    void ShowTooltipIfHovered(const std::string& text);

    // TODO: replace this not good place
    std::string readFile(const std::filesystem::path& filepath);
}
}