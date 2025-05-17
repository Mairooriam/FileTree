#pragma once
#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <memory>

#include "FileNode.h"

namespace fs = std::filesystem;

enum class SortCriteria {
    TypeThenName,        // Default: folders first, then by name
    Extension,           // Group by extension
    Name,                // Just by name (case insensitive)
    Size,                // By file size
    DateModified         // By modification date
};

class FileTree
{
private:
    std::unique_ptr<FileNode> m_rootNode;
    FileNode* m_currentNode = nullptr; 

    int m_maxDepth = -1;
    SortCriteria m_sortCriteria = SortCriteria::TypeThenName;
    std::unique_ptr<FileNode> buildFileTree(const fs::path& folder);
    void printFileTree(FileNode* _node, int depth = 0);
    void sortChildren(FileNode* node);
public:
    FileTree();
    explicit FileTree(const fs::path& folder);
    ~FileTree();
    
    void setRootFolder(const fs::path& _folder);
    void setSortCriteria(SortCriteria criteria);
    
    void print();
    void refreshRootNode();
    bool expandNode(FileNode* node);
    
    fs::path getCurrentPath() const;
    std::vector<FileNode*> getCurrentChildren() const;
    SortCriteria getSortCriteria() const { return m_sortCriteria; }
    fs::path getRootFolder() { return m_rootNode.get()->fullPath; }
    FileNode* getRootNode() const { return m_rootNode.get(); }
    
    bool isInitialized() const {return m_rootNode != nullptr;}
};