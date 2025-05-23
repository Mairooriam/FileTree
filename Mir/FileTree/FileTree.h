#pragma once
#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <memory>
#include <list>
#include <unordered_map>
#include "FileNode.h"

// TODO: double click file not working deep into tree. callback yes but the file editor thing

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
    std::list<fs::path> m_dirHistory;
    int m_maxDepth = -1;
    SortCriteria m_sortCriteria = SortCriteria::TypeThenName;
    std::unique_ptr<FileNode> buildFileTree(const fs::path& folder);
    void printFileTree(FileNode* _node, int depth = 0);
    void sortChildren(FileNode* node);

    static std::unordered_map<SortCriteria, std::string> s_sortCriteriaStrings;
    static std::unordered_map<std::string, SortCriteria> s_stringToSortCriteria;
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
    const std::list<fs::path>& getDirHistory() const { return m_dirHistory; }; 
    std::vector<FileNode*> getCurrentChildren() const;
    SortCriteria getSortCriteria() const { return m_sortCriteria; }
    static const std::unordered_map<SortCriteria, std::string>& GetAllSortCriteriaStrings() { return s_sortCriteriaStrings; }
    fs::path getRootFolder() { return m_rootNode.get()->fullPath; }
    FileNode* getRootNode() const { return m_rootNode.get(); }
    
    bool isInitialized() const {return m_rootNode != nullptr;}


    
};