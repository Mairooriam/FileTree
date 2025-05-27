#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
namespace FTree{
enum class FileType { DIR, FILE, UNKNOWN };

class FileNodeVisitor;

struct FileNode {
    std::vector<std::unique_ptr<FileNode>> children;
    std::filesystem::path fullPath;
    std::wstring name;
    bool isVisible = true;  // helper for UI.
    FileType type = FileType::UNKNOWN;
    size_t size = 0;
    bool hasUnexpandedChildren = false;

    FileNode() {}
    FileNode(const std::wstring& nodeName, FileType nodeType) : name(nodeName), type(nodeType) {}
    ~FileNode() {}

    FileNode(const FileNode& other, bool copyChildren = true);
    FileNode(FileNode&& other) noexcept;
    FileNode& operator=(const FileNode& other);
    FileNode& operator=(FileNode&& other) noexcept;

    std::wstring getExtension() const;
    void addChild(std::unique_ptr<FileNode> child) { children.push_back(std::move(child)); }

private:
    friend std::ostream& operator<<(std::ostream& os, const FileNode& node);
};
}