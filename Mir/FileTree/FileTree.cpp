#include "FileTree.h"
#include "FileNode.h"
#include "FileTree.h"
#include <functional>
#include <algorithm>

std::unordered_map<SortCriteria, std::string> FileTree::s_sortCriteriaStrings = {
    {SortCriteria::TypeThenName, "Type then Name"},
    {SortCriteria::Extension, "Extension"},
    {SortCriteria::Name, "Name"},
    {SortCriteria::Size, "Size"},
    {SortCriteria::DateModified, "Date Modified"}
};

std::unordered_map<std::string, SortCriteria> FileTree::s_stringToSortCriteria = {
    {"Type then Name", SortCriteria::TypeThenName},
    {"Extension", SortCriteria::Extension},
    {"Name", SortCriteria::Name},
    {"Size", SortCriteria::Size},
    {"Date Modified", SortCriteria::DateModified}
};

FileTree::~FileTree() {}

FileTree::FileTree() : FileTree(fs::current_path()) {}

FileTree::FileTree(const fs::path& _folder) {
    m_rootNode = buildFileTree(_folder);
    m_currentNode = m_rootNode.get(); 
}

std::unique_ptr<FileNode> FileTree::buildFileTree(const fs::path& _folder) {
    auto filename = _folder.filename().empty() ? fs::path(_folder).wstring() : _folder.filename().wstring();
    auto rootNode = std::make_unique<FileNode>(filename, FileType::DIR);
    
    rootNode->fullPath = _folder;
    
    try {
        if (!fs::exists(_folder)) {
            return rootNode;
        }
        
        rootNode->children.reserve(16);
        
        std::error_code ec; // Use error_code to avoid exceptions (faster) copilot :)
        for (const auto& entry : fs::directory_iterator(_folder, ec)) {
            if (ec) {
                break;
            }
            
            try {
                auto filePath = entry.path();
                
                if (fs::is_directory(entry, ec)) {
                    auto childNode = std::make_unique<FileNode>(filePath.filename().wstring(), FileType::DIR);
                    childNode->fullPath = filePath;
                    childNode->hasUnexpandedChildren = true; 
                    rootNode->addChild(std::move(childNode));
                }
                else if (fs::is_regular_file(entry, ec)) {
                    auto fileNode = std::make_unique<FileNode>(filePath.filename().wstring(), FileType::FILE);
                    fileNode->size = fs::file_size(entry, ec);
                    fileNode->fullPath = filePath;
                    rootNode->addChild(std::move(fileNode));
                }
            }
            catch (const std::exception&) { } 
        }
    }
    catch (const std::exception&) { }
    sortChildren(rootNode.get());
    return rootNode;
}

void FileTree::printFileTree(FileNode* _node, int _depth) {
    std::wstring indent(_depth * 2, L' ');
    
    std::wcout << indent;
    if (_node->type == FileType::DIR) {
        std::wcout << L"[DIR] " << _node->name << std::endl;
    } else {
        std::wcout << L"[FILE] " << _node->name << L" (" << _node->size << L" bytes)" << std::endl;
    }
    
    for (const auto& child : _node->children) {
        printFileTree(child.get(), _depth + 1);
    }
}
void FileTree::print() {
    printFileTree(m_rootNode.get());
}
void FileTree::setRootFolder(const fs::path& _folder) {
    if (!_folder.empty())
    {
        auto oldDir = getRootFolder();
        auto it = std::find(m_dirHistory.begin(), m_dirHistory.end(), oldDir);
        if (it != m_dirHistory.end()) {
            m_dirHistory.erase(it);
        }
        
        m_dirHistory.insert(m_dirHistory.begin(), oldDir);


        m_rootNode = buildFileTree(_folder);
        m_currentNode = m_rootNode.get(); 
    }else{
        std::cout << "[FileTree::setRootFolder] tried to set empty root path" << "\n";
    }
    
}


bool FileTree::expandNode(FileNode* node) {
    if (!node || node->type != FileType::DIR || !node->hasUnexpandedChildren) {
        return false;
    }
    node->children.clear();
    node->hasUnexpandedChildren = false;
    
    try {
        std::error_code ec;
        
        for (const auto& entry : fs::directory_iterator(node->fullPath, ec)) {
            auto filePath = entry.path();
            
            if (fs::is_directory(entry, ec)) {
                auto childNode = std::make_unique<FileNode>(filePath.filename().wstring(), FileType::DIR);
                childNode->fullPath = filePath;
                childNode->hasUnexpandedChildren = true;
                node->addChild(std::move(childNode));
            }
            else if (fs::is_regular_file(entry, ec)) {
                auto fileNode = std::make_unique<FileNode>(filePath.filename().wstring(), FileType::FILE);
                fileNode->size = fs::file_size(entry, ec);
                fileNode->fullPath = filePath;
                node->addChild(std::move(fileNode));
            }
        }
        
        sortChildren(node);
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error expanding node: " << e.what() << std::endl;
        return false;
    }
}

std::vector<FileNode*> FileTree::getCurrentChildren() const {
    std::vector<FileNode*> result;
    if (!m_currentNode) return result;
    
    for (const auto& child : m_currentNode->children) {
        result.push_back(child.get());
    }
    return result;
}

fs::path FileTree::getCurrentPath() const {
    return m_currentNode ? m_currentNode->fullPath : fs::path();
}

void FileTree::setSortCriteria(SortCriteria criteria) {
    if (m_sortCriteria != criteria) {
        m_sortCriteria = criteria;
        // Resort current node if it exists
        if (m_currentNode) {
            sortChildren(m_currentNode);
        }
    }
}

void FileTree::refreshRootNode() {
    fs::path currentPath = m_rootNode->fullPath;
    m_rootNode = buildFileTree(currentPath);
    m_currentNode = m_rootNode.get();
}

void FileTree::sortChildren(FileNode* node) {
    if (!node || node->children.empty()) return;
    
    switch (m_sortCriteria) {
        case SortCriteria::TypeThenName:
            std::sort(node->children.begin(), node->children.end(), 
                [](const std::unique_ptr<FileNode>& a, const std::unique_ptr<FileNode>& b) {
                    if (a->type != b->type) {
                        return a->type == FileType::DIR;
                    }
                    
                    std::wstring nameA = a->name;
                    std::wstring nameB = b->name;
                    
                    std::transform(nameA.begin(), nameA.end(), nameA.begin(), ::towlower);
                    std::transform(nameB.begin(), nameB.end(), nameB.begin(), ::towlower);
                    
                    return nameA < nameB;
                });
            break;
            
        case SortCriteria::Extension:
            std::sort(node->children.begin(), node->children.end(), 
                [](const std::unique_ptr<FileNode>& a, const std::unique_ptr<FileNode>& b) {
                    if (a->type != b->type) {
                        return a->type == FileType::DIR;
                    }
                    
                    if (a->type == FileType::FILE) {
                        auto extA = a->fullPath.extension().wstring();
                        auto extB = b->fullPath.extension().wstring();
                        
                        std::transform(extA.begin(), extA.end(), extA.begin(), ::towlower);
                        std::transform(extB.begin(), extB.end(), extB.begin(), ::towlower);
                        
                        if (extA != extB) {
                            return extA < extB;
                        }
                    }
                    
                    std::wstring nameA = a->name;
                    std::wstring nameB = b->name;
                    
                    std::transform(nameA.begin(), nameA.end(), nameA.begin(), ::towlower);
                    std::transform(nameB.begin(), nameB.end(), nameB.begin(), ::towlower);
                    
                    return nameA < nameB;
                });
            break;
            
        case SortCriteria::Name:
            std::sort(node->children.begin(), node->children.end(), 
                [](const std::unique_ptr<FileNode>& a, const std::unique_ptr<FileNode>& b) {
                    std::wstring nameA = a->name;
                    std::wstring nameB = b->name;
                    
                    std::transform(nameA.begin(), nameA.end(), nameA.begin(), ::towlower);
                    std::transform(nameB.begin(), nameB.end(), nameB.begin(), ::towlower);
                    
                    return nameA < nameB;
                });
            break;
            
        case SortCriteria::Size:
            std::sort(node->children.begin(), node->children.end(), 
                [](const std::unique_ptr<FileNode>& a, const std::unique_ptr<FileNode>& b) {
                    if (a->type != b->type) {
                        return a->type == FileType::DIR;
                    }
                    
                    if (a->type == FileType::FILE) {
                        if (a->size != b->size) {
                            return a->size > b->size; // Descending order
                        }
                    }
                    
                    std::wstring nameA = a->name;
                    std::wstring nameB = b->name;
                    
                    std::transform(nameA.begin(), nameA.end(), nameA.begin(), ::towlower);
                    std::transform(nameB.begin(), nameB.end(), nameB.begin(), ::towlower);
                    
                    return nameA < nameB;
                });
            break;

    }
}

