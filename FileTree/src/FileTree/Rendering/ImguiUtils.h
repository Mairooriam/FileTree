#pragma once
#include "imgui.h"
#include <string>
#include <filesystem>

namespace ImGuiUtils {
    void ShowTooltipIfHovered(const std::string& text);
}