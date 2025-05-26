#include "IFileDialogManager.h"
#include "WindowsFileDialog.h"
namespace Mir
{
    std::unique_ptr<IFileDialogManager> IFileDialogManager::Create() {
        return std::make_unique<WindowsFileDialog>();
    }
} // namespace Mir
