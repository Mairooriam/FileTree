#include "FilterCommand.h"
#include <functional>
#include "FileTree/commands/filetree/FilterCommand.h"
#include "visitor/FileTreeVisitorBase.h"

std::function<bool(const FileNode*)> CreateFilterPredicate(
    const std::vector<std::string>& extensions) {
    return [extensions](const FileNode* node) -> bool {
        if (node->type != FileType::FILE)
            return false;

        std::filesystem::path filePath(node->fullPath);
        std::string ext = filePath.extension().string();
        std::transform(
            ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });

        // Check if the extension is in the provided vector
        return std::find(extensions.begin(), extensions.end(), ext) != extensions.end();
    };
}
bool ResetNodeVisibility(const FileNode* node) {
    const_cast<FileNode*>(node)->isVisible = true;
    return true;  // Continue traversal
}



ApplyFilterCmd::ApplyFilterCmd(std::shared_ptr<FileTree> _filetree,
                               const std::vector<std::string>& _extensions)
    : m_filetree{_filetree}  
    {
        if (_extensions.empty())
        {
            m_filterPredicate = ResetNodeVisibility;
        } else{

            m_filterPredicate = CreateFilterPredicate(_extensions);
        }
        
    }

void ApplyFilterCmd::execute() {
    VisibilityFilterVisitor filter(m_filterPredicate);
    FileTreeVisitor visiter(filter);
    visiter.traverse(m_filetree->getRootNode());
}

void ApplyFilterCmd::undo() {
    std::cout << "Not implemenetedred" << "\n";
}

std::string ApplyFilterCmd::getName() const {
    return "[ApplyFilterCmd] executed" ;
}
std::string ResetFilterCmd::getName() const {
    return "[ResetFilterCmd] Executed";
}


ResetFilterCmd::ResetFilterCmd(std::shared_ptr<FileTree> _filetree) :m_filetree(_filetree) {}

void ResetFilterCmd::execute() {

    VisibilityFilterVisitor filter(ResetNodeVisibility);
    FileTreeVisitor visiter(filter);
    visiter.traverse(m_filetree->getRootNode());
}
void ResetFilterCmd::undo() {
    std::cout << "no undo implemented" << "\n";
}
