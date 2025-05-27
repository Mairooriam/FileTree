#include "Rendering/IFileDialogManager.h"
#include "WindowsFileDialog.h"
namespace FTree {
std::unique_ptr<IFileDialogManager> IFileDialogManager::Create() {
    return std::make_unique<WindowsFileDialog>();
}
}  // namespace FTree
