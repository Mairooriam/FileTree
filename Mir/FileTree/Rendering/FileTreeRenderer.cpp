#include <functional>
#include "imgui.h"
#include "imgui_stdlib.h"

#include "ImguiUtils.h"
#include "utils/Utils.h"
#include "visitor/FileTreeVisitorBase.h"
#include "FileTreeRenderer.h"

#include "commands/commands.h"
#include "commands/filetree/FilterCommand.h"



FileTreeRenderer::FileTreeRenderer(const std::shared_ptr<FileTree>& _fileTree)
    : m_FileTree{_fileTree}, m_fileDialog{Mir::IFileDialogManager::Create()} {}

//--------------------------------------------------------------------------------------------------------------------------------------------------
// Rendering START
//--------------------------------------------------------------------------------------------------------------------------------------------------
void FileTreeRenderer::Render(){
    ImGui::ShowDemoWindow();
    
    auto rootFolder = m_FileTree->getRootFolder().string();
    ImGui::Begin("File Tree", nullptr, ImGuiWindowFlags_MenuBar);
    RenderMenuBar();
   
    if (m_FileTree->state_Check(StateFlags::STATE_NEW_NODE_EXPLORED))
    {
        ExtensionCollectorVisitor collector;
        FileTreeVisitor visitor(collector);
        visitor.traverse(m_FileTree->getRootNode());
        auto& manager = CommandManager::getInstance();
        auto cmd = std::make_unique<ApplyFilterCmd>(m_FileTree, getSelectedExtensions());
        manager.execute(std::move(cmd));
        m_FileTree->state_Clear(StateFlags::STATE_NEW_NODE_EXPLORED);
    }else if (m_FileTree->state_Check(StateFlags::STATE_FILTERS_UPDATED))
    {
        auto& manager = CommandManager::getInstance();
        auto cmd = std::make_unique<ApplyFilterCmd>(m_FileTree, getSelectedExtensions());
        manager.execute(std::move(cmd));
        m_FileTree->state_Clear(StateFlags::STATE_FILTERS_UPDATED);
    }
    

    



    
    ImGui::Separator();
    
    // Render each node in file tree
    if (m_FileTree && m_FileTree->isInitialized()) {
        RenderFileNode(m_FileTree->getRootNode());
    } else{
        ImGui::Text("File tree not initialized. Click 'Update File Tree' to load.");
    }
    ImGui::End();
    
    if (!m_CurrentOpenFile.content.empty() && !m_CurrentOpenFile.path.empty())
    {
        RenderOpenFile();
    }
    
}

void FileTreeRenderer::RenderFileNode(FileNode* _node) {
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    
   if (_node && !_node->isVisible) {
        return;
    }

    if (_node->type == FileType::FILE || _node->type == FileType::UNKNOWN) {
        flags |= ImGuiTreeNodeFlags_Leaf;       
        flags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;  
    }
    
    if (_node == m_FileTree->getRootNode()) {
        flags |= ImGuiTreeNodeFlags_DefaultOpen;
    }
    
    std::string icon;
    std::string nodeName = std::string(_node->name.begin(), _node->name.end());
    std::string displayName;
    
    if (_node->type == FileType::DIR) {
        icon = "[DIR] ";
        displayName = icon + nodeName;
    } else if (_node->type == FileType::FILE) {
        icon = "[FILE] ";
        std::string sizeStr = formatFileSize(_node->size);
        displayName = icon + nodeName + " (" + sizeStr + ")";
    }
    
    
    std::string id = "##" + displayName + std::to_string(reinterpret_cast<uintptr_t>(_node));
    bool nodeOpen = ImGui::TreeNodeEx((displayName + id).c_str(), flags);
    RenderFileTreeContextMenu(_node);
    HandleDoubleClickNode(_node);
    HandleSingleClickNode(_node);
    
    // Lazy loading: when a directory node is expanded for the first time
    if (nodeOpen && _node->type == FileType::DIR) {
        
        for (const auto& child : _node->children) {
            RenderFileNode(child.get());
        }
        if (_node->hasUnexpandedChildren) {
            m_FileTree->expandNode(_node);
        }
        ImGui::TreePop();
    }
}

void FileTreeRenderer::RenderOpenFile() 
{
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    
    ImGui::Begin(m_CurrentOpenFile.path.c_str(), nullptr, 
    ImGuiWindowFlags_HorizontalScrollbar | 
    ImGuiWindowFlags_NoBringToFrontOnFocus);
    
    ImGui::SameLine();
    if (ImGui::Button("Close")) {
        m_CurrentOpenFile.content.clear();
        m_CurrentOpenFile.path.clear();
        ImGui::End();
        return;
    }
    
    ImGui::Separator();
    
    ImVec2 availSize = ImGui::GetContentRegionAvail();
    
    ImGui::InputTextMultiline("##FileContent", 
        &m_CurrentOpenFile.content, 
        ImVec2(availSize.x, availSize.y), 
        ImGuiInputTextFlags_AllowTabInput);
        
        ImGui::End();
}

void FileTreeRenderer::RenderFileTreeContextMenu(FileNode* _node) {
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Copy Path")) {
            fs::path fullPath = m_FileTree->getRootNode()->fullPath / _node->name;
            std::string pathStr = fullPath.string();
          
            ImGui::SetClipboardText(pathStr.c_str());
        }
        
        if (_node->type == FileType::FILE && ImGui::MenuItem("Open File")) {
            fs::path fullPath = m_FileTree->getRootNode()->fullPath / _node->name;
            std::string pathStr = fullPath.string();
            m_CurrentOpenFile.content = Mir::Utils::File::readFile(fullPath);
            m_CurrentOpenFile.path = pathStr;

        }

        if (_node->type == FileType::FILE) {
            std::string ext = _node->fullPath.extension().string();
            std::string loweredExt = ext;
            std::transform(loweredExt.begin(), loweredExt.end(), loweredExt.begin(),
                          [](unsigned char c) { return std::tolower(c); });
            
            if (m_callbackRegistry.count(loweredExt) > 0 && 
                m_callbackRegistry[loweredExt].count(CallbackType::ContextMenu) > 0) {
                
                std::string menuLabel = "Process " + ext.substr(1) + " file"; // Remove the dot
                if (ImGui::MenuItem(menuLabel.c_str())) {
                    m_callbackRegistry[loweredExt][CallbackType::ContextMenu](_node->fullPath.string());
                }
            }
        }
        if (_node->type == FileType::DIR && ImGui::MenuItem("New File"))
        {
            std::cout << "[Not implemented!]" << "\n";
        }
        
        ImGui::EndPopup();
    }



}









void FileTreeRenderer::RenderMenuBar() {
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                m_FileTree->setRootFolder(OpenFolderDialog()); 
            }
            if (ImGui::BeginMenu("Open Recent"))
            {
                if (m_FileTree->getDirHistory().empty()) {
                    // Display text when no history exists
                    ImGui::Text("No history");
                } else {
                    // Display history items as before
                    for (auto &&dir : m_FileTree->getDirHistory())
                    {
                        if(ImGui::MenuItem(dir.string().c_str())){
                            m_FileTree->setRootFolder(dir);
                        };
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();
            ImGui::EndMenu(); // Added missing EndMenu call for "File" menu
        }
        if (ImGui::BeginMenu("Sorting"))
        {
            auto filters = m_FileTree->GetAllSortCriteriaStrings();
            for (const auto& [criteria, label] : filters) // C++17 structured binding
            {
                // Create a checkable menu item that shows which option is currently selected
                bool isSelected = (m_FileTree->getSortCriteria() == criteria);
                if (ImGui::MenuItem(label.c_str(), NULL, isSelected)) {
                    // When clicked, update the sort criteria
                    m_FileTree->setSortCriteria(criteria);
                }
            }
            ImGui::EndMenu(); // This is correct now since we're using BeginMenu
        }
            
        if (ImGui::BeginMenu("Filters"))
        {
            if (ImGui::Button("Select All")) {
                for (auto& [ext, selected] : m_selectedExtensions) {
                    selected = true;
                }
                m_FileTree->state_Set(StateFlags::STATE_FILTERS_UPDATED);
            }
            if (ImGui::Button("Deselect All")) {
                for (auto& [ext, selected] : m_selectedExtensions) {
                    selected = false;
                }
                m_FileTree->state_Set(StateFlags::STATE_FILTERS_UPDATED);
            }
            
            ImGui::Separator();
            
            ExtensionCollectorVisitor collector;
            FileTreeVisitor visitor(collector);
            visitor.traverse(m_FileTree->getRootNode());
            ImGui::BeginChild("FiltersList", ImVec2(0, 300), true);

            for (auto &&ext : collector.getExtensions())
            {
                if (m_selectedExtensions.find(ext) == m_selectedExtensions.end()) {
                    m_selectedExtensions[ext] = false;
                }
                
                bool isSelected = m_selectedExtensions[ext];
                
                // Use checkbox instead of selectable to prevent menu closing
                if (ImGui::Checkbox(ext.c_str(), &m_selectedExtensions[ext])) {
                    m_FileTree->state_Set(StateFlags::STATE_FILTERS_UPDATED);
                }
            }

            ImGui::EndChild();



            ImGui::EndMenu();// This is correct now since we're using BeginMenu
        }
        
        ImGui::EndMenuBar();
    }
}
std::vector<std::string> FileTreeRenderer::getSelectedExtensions() {
    std::vector<std::string> selectedExts;
    for (const auto& [extension, isSelected] : m_selectedExtensions) {
        if (isSelected) {
            selectedExts.push_back(extension);
        }
    }
    return selectedExts;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
// Rendering END
//--------------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------------------------
// Helpers START
//--------------------------------------------------------------------------------------------------------------------------------------------------
std::string FileTreeRenderer::formatFileSize(size_t sizeInBytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    double size = static_cast<double>(sizeInBytes);
    
    while (size >= 1024.0 && unitIndex < 4) {
        size /= 1024.0;
        unitIndex++;
    }
    
    char buffer[32];
    if (unitIndex == 0) {
        snprintf(buffer, sizeof(buffer), "%zu %s", sizeInBytes, units[unitIndex]);
    } else if (size < 10) {
        snprintf(buffer, sizeof(buffer), "%.2f %s", size, units[unitIndex]);
    } else if (size < 100) {
        snprintf(buffer, sizeof(buffer), "%.1f %s", size, units[unitIndex]);
    } else {
        snprintf(buffer, sizeof(buffer), "%.0f %s", size, units[unitIndex]);
    }
    
    return buffer;
}

std::filesystem::path FileTreeRenderer::OpenFileDialog()  {
    std::filesystem::path result;
    m_fileDialog->SetInitialPath(m_FileTree->getRootFolder());
    m_fileDialog->OpenFileDialog("Open File", [&result](const std::filesystem::path& path) {
        if (!path.empty()) {result = path;}
    });
    return result;
}

std::filesystem::path FileTreeRenderer::OpenFolderDialog() {
    std::filesystem::path result;
    m_fileDialog->SetInitialPath(m_FileTree->getRootFolder());
    m_fileDialog->OpenFolderDialog("Open Folder", [&result](const std::filesystem::path& path) {      
        if (!path.empty()) { 
            result = path;
        }
    });
    
    return result;
}
void FileTreeRenderer::HandleDoubleClickNode(FileNode* _node) {
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        fs::path fullPath = m_FileTree->getRootNode()->fullPath / _node->name;
        std::string pathStr = fullPath.string();

        if (_node->type == FileType::FILE) {
            m_CurrentOpenFile.content = Mir::Utils::File::readFile(fullPath);
            m_CurrentOpenFile.path = pathStr;

            TriggerFileCallback(CallbackType::DoubleClick, pathStr);
            std::string ext = _node->fullPath.extension().string();
            TriggerExtensionCallback(ext, CallbackType::DoubleClick, pathStr);
        } else if (_node->type == FileType::DIR) {
            TriggerDirectoryCallback(CallbackType::DoubleClick, pathStr);
        }
    }
}

void FileTreeRenderer::HandleSingleClickNode(FileNode* _node) {
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
        auto path = _node->fullPath.string();
        if (_node->type == FileType::FILE) {
            TriggerFileCallback(CallbackType::Click, path);
            
            std::string ext = _node->fullPath.extension().string();
            TriggerExtensionCallback(ext, CallbackType::Click, path);
        }
        else if (_node->type == FileType::DIR) {
            TriggerDirectoryCallback(CallbackType::Click, path);
        }
    }
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
// Helpers END
//--------------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------------------------
// Callback START
//--------------------------------------------------------------------------------------------------------------------------------------------------
void FileTreeRenderer::RegisterFileCallback(CallbackType type, NodeCallback callback) {
        m_fileCallbacks[type] = callback;
}

void FileTreeRenderer::RegisterDirectoryCallback(CallbackType type, NodeCallback callback) {
        m_dirCallbacks[type] = callback;
}

void FileTreeRenderer::RegisterExtensionCallback(const std::string& extension, CallbackType type,
                                                 NodeCallback callback) {
        std::string ext = extension;
        std::transform(ext.begin(), ext.end(), ext.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        m_callbackRegistry[ext][type] = callback;
}

void FileTreeRenderer::TriggerFileCallback(CallbackType type, const std::filesystem::path& path) {
        if (m_fileCallbacks.count(type) > 0) {
            m_fileCallbacks[type](path);
        }
}

void FileTreeRenderer::TriggerDirectoryCallback(CallbackType type, const std::filesystem::path& path) {
        if (m_dirCallbacks.count(type) > 0) {
            m_dirCallbacks[type](path);
        }
}

void FileTreeRenderer::TriggerExtensionCallback(const std::string& extension, CallbackType type,
                                                const std::filesystem::path& path) {
        std::string ext = extension;
        std::transform(ext.begin(), ext.end(), ext.begin(),
                      [](unsigned char c) { return std::tolower(c); });
                      
        if (m_callbackRegistry.count(ext) > 0 && m_callbackRegistry[ext].count(type) > 0) {
            m_callbackRegistry[ext][type](path);
        }
}


//--------------------------------------------------------------------------------------------------------------------------------------------------
// Callback END
//--------------------------------------------------------------------------------------------------------------------------------------------------
    
