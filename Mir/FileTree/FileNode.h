#pragma once
#include <string>
#include <vector>
#include <memory>
enum class FileType {
    DIR,
    FILE,
    UNKNOWN
};

class FileNodeVisitor;
struct FileNode {
    std::vector<std::unique_ptr<FileNode>> children;
    std::filesystem::path fullPath; 
    std::wstring name;
    
    FileType type = FileType::UNKNOWN;
    size_t size = 0; 
    bool hasUnexpandedChildren = false; 
   
    FileNode() { }
    ~FileNode() {}
   
   std::wstring getExtension() const {
        if (type != FileType::FILE) {
            return L"";
        }
        
        size_t dotPos = name.find_last_of(L'.');
        if (dotPos != std::wstring::npos) {
            return name.substr(dotPos);
        }
        return L"";
    }


    FileNode(const std::wstring& nodeName, FileType nodeType) 
    : name(nodeName), type(nodeType) {}
    
    void addChild(std::unique_ptr<FileNode> child) {
        children.push_back(std::move(child));
    }
    
    void accept(FileNodeVisitor& visitor);
};