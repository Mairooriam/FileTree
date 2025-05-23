#pragma once
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
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

    friend std::ostream& operator<<(std::ostream& os, const FileNode& node) {
        return printToStream(os, node);
    }
    
    friend std::wostream& operator<<(std::wostream& os, const FileNode& node) {
        return printToWStream(os, node);
    }

    FileNode(const FileNode& other)
        : name(other.name),
          fullPath(other.fullPath),
          type(other.type),
          size(other.size),
          hasUnexpandedChildren(other.hasUnexpandedChildren) 
    {
        // Deep copy children
        for (const auto& child : other.children) {
            if (child) {
                children.push_back(std::make_unique<FileNode>(*child));
            }
        }
    }
    
    // Add move constructor
    FileNode(FileNode&& other) noexcept
        : children(std::move(other.children)),
          fullPath(std::move(other.fullPath)),
          name(std::move(other.name)),
          type(other.type),
          size(other.size),
          hasUnexpandedChildren(other.hasUnexpandedChildren)
    {
    }
    
    // Add copy assignment operator
    FileNode& operator=(const FileNode& other) {
        if (this != &other) {
            name = other.name;
            fullPath = other.fullPath;
            type = other.type;
            size = other.size;
            hasUnexpandedChildren = other.hasUnexpandedChildren;
            
            // Clear existing children
            children.clear();
            
            // Deep copy children
            for (const auto& child : other.children) {
                if (child) {
                    children.push_back(std::make_unique<FileNode>(*child));
                }
            }
        }
        return *this;
    }
    
    // Add move assignment operator
    FileNode& operator=(FileNode&& other) noexcept {
        if (this != &other) {
            children = std::move(other.children);
            fullPath = std::move(other.fullPath);
            name = std::move(other.name);
            type = other.type;
            size = other.size;
            hasUnexpandedChildren = other.hasUnexpandedChildren;
        }
        return *this;
    }
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
    
    private:
    // Private recursive helper for standard stream
    static std::ostream& printToStream(std::ostream& os, const FileNode& node, int depth = 0) {
        std::string indent(depth * 2, ' ');
        std::string nameStr(node.name.begin(), node.name.end());
        
        os << indent;
        
        if (node.type == FileType::DIR) {
            os << "[DIR] ";
        } else if (node.type == FileType::FILE) {
            os << "[FILE] ";
        } else {
            os << "[UNKNOWN] ";
        }
        
        os << nameStr;
        
        if (node.type == FileType::FILE) {
            os << " (" << node.size << " bytes)";
        }
        
        os << '\n';
        
        // Recursively print children
        for (const auto& child : node.children) {
            if (child) {
                printToStream(os, *child, depth + 1);
            }
        }
        
        return os;
    }
    
    // Private recursive helper for wide stream
    static std::wostream& printToWStream(std::wostream& os, const FileNode& node, int depth = 0) {
        std::wstring indent(depth * 2, L' ');
        
        os << indent;
        
        if (node.type == FileType::DIR) {
            os << L"[DIR] ";
        } else if (node.type == FileType::FILE) {
            os << L"[FILE] ";
        } else {
            os << L"[UNKNOWN] ";
        }
        
        os << node.name;
        
        if (node.type == FileType::FILE) {
            os << L" (" << node.size << L" bytes)";
        }
        
        os << L'\n';
        
        // Recursively print children
        for (const auto& child : node.children) {
            if (child) {
                printToWStream(os, *child, depth + 1);
            }
        }
        
        return os;
    }


};