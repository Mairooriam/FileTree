#pragma once
#include <string>
#include <functional>
#include <filesystem>
#include <memory>

using FileDialogCallback = std::function<void(const std::filesystem::path&)>;
namespace Mir{
    class IFileDialogManager {
    public:
        virtual ~IFileDialogManager() = default;
        
        virtual void OpenFolderDialog(const std::string& title, FileDialogCallback callback) = 0;
        
        virtual void OpenFileDialog(const std::string& title, FileDialogCallback callback, 
                                   const std::string& filter = "") = 0;
                                   
        virtual void SetInitialPath(const std::filesystem::path& path) = 0;
        
        static std::unique_ptr<IFileDialogManager> Create();
    };
}