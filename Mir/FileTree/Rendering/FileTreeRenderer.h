#pragma once
#include "FileTree.h"
#include "IFileDialogManager.h"
#include <functional>
#include <map>
#include <set>
enum class FileOpenMode {
    Text,           // Plain text editor
    Binary,         // Hex/binary viewer
    Image,          // Image viewer
    Code,           // Syntax-highlighted code editor
    Structured,     // JSON, XML, YAML viewer with formatting
    Unsupported     // Cannot be opened
};

class FileTreeRenderer
{
    public:
    void Render();
    FileTreeRenderer(const std::shared_ptr<FileTree>& _fileTree);
    ~FileTreeRenderer() {}      
    enum class CallbackType {
        Click,
        DoubleClick,
        ContextMenu
    };
    
    private:
    std::shared_ptr<FileTree> m_FileTree;
    std::unique_ptr<Mir::IFileDialogManager> m_fileDialog;
    struct OpenFile{
        std::string content;
        std::string path;
    }m_CurrentOpenFile;

    std::unordered_map<std::string, bool> m_selectedExtensions;
    std::set<std::string> m_cachedExtensions;
private:
    void RenderOpenFile();
    void RenderFileNode(FileNode* _fileNode);
    void RenderFileTreeContextMenu(FileNode* _node);
    void RenderMenuBar();
    
    std::string formatFileSize(size_t sizeInBytes);
    
    std::filesystem::path OpenFileDialog();
    std::filesystem::path OpenFolderDialog();
    void HandleMappingTypFile();
    void HandleMappingCsvFile();
    void HandleDoubleClickNode(FileNode* _node);
    void HandleSingleClickNode(FileNode* _node);

private: 
    using NodeCallback = std::function<void(const std::filesystem::path& path)>;
    std::map<std::string, std::map<CallbackType, NodeCallback>> m_callbackRegistry;
    
    std::map<CallbackType, NodeCallback> m_fileCallbacks;
    std::map<CallbackType, NodeCallback> m_dirCallbacks;
    void TriggerFileCallback(CallbackType type, const std::filesystem::path& path); 
    void TriggerDirectoryCallback(CallbackType type, const std::filesystem::path& path); 
    void TriggerExtensionCallback(const std::string& extension, CallbackType type, const std::filesystem::path& path); 
public:    
    void RegisterFileCallback(CallbackType type, NodeCallback callback); 
    void RegisterDirectoryCallback(CallbackType type, NodeCallback callback); 
    void RegisterExtensionCallback(const std::string& extension, CallbackType type, NodeCallback callback); 
private:
    // States
    enum StateFlags : uint32_t {
        STATE_NONE = 0,
        STATE_EXTENSIONS_CACHED = 1 << 0,
        STATE_FILTER_APPLIED = 1 << 1,
        STATE_TREE_MODIFIED = 1 << 2,
        STATE_FILE_OPEN = 1 << 3,
        STATE_PENDING_REFRESH = 1 << 4,
        STATE_SHOW_HIDDEN_FILES = 1 << 5,
        STATE_SORTING_CHANGED = 1 << 6,
    };
    
    uint32_t m_stateFlags = STATE_NONE;
    
    bool CheckState(StateFlags flag) const { return (m_stateFlags & flag) != 0; }
    void SetState(StateFlags flag) { m_stateFlags |= flag; }
    void ClearState(StateFlags flag) { m_stateFlags &= ~flag; }
    void ToggleState(StateFlags flag) { m_stateFlags ^= flag; }
};
