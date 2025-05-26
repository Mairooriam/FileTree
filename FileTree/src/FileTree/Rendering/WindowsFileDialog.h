#pragma once
#include "IFileDialogManager.h"
#include <windows.h>
#include <shobjidl.h>
namespace Mir {
    // straight from copilot
    class WindowsFileDialog : public IFileDialogManager {
    public:
        WindowsFileDialog();
        ~WindowsFileDialog() override;
        
        void OpenFolderDialog(const std::string& title, FileDialogCallback callback) override;
        void OpenFileDialog(const std::string& title, FileDialogCallback callback, 
                           const std::string& filter = "") override;
        void SetInitialPath(const std::filesystem::path& path) override;
        
    private:
        bool m_isInitialized;
        std::wstring m_initialPath;
        
        bool InitializeCOM();
        void ReleaseCOM();

        std::wstring Utf8ToWide(const std::string& str);
    };
}