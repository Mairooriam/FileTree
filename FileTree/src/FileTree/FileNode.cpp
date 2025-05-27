#include <FileNode.h>
namespace FTree {
// Add this constructor to your FileNode class
FileNode::FileNode(const FileNode& other, bool copyChildren)
    : name(other.name),
      fullPath(other.fullPath),
      type(other.type),
      size(other.size),
      hasUnexpandedChildren(other.hasUnexpandedChildren),
      isVisible(other.isVisible) {
    // Only deep copy children if requested
    if (copyChildren) {
        for (const auto& child : other.children) {
            if (child) {
                children.push_back(std::make_unique<FileNode>(*child));
            }
        }
    }
}
FileNode::FileNode(FileNode&& other) noexcept
    : children(std::move(other.children)),
      fullPath(std::move(other.fullPath)),
      name(std::move(other.name)),
      type(other.type),
      size(other.size),
      hasUnexpandedChildren(other.hasUnexpandedChildren),
      isVisible(other.isVisible) {}

FileNode& FileNode::operator=(const FileNode& other) {
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
FileNode& FileNode::operator=(FileNode&& other) noexcept {
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
std::wstring FileNode::getExtension() const {
    if (type != FileType::FILE) {
        return L"";
    }

    size_t dotPos = name.find_last_of(L'.');
    if (dotPos != std::wstring::npos) {
        return name.substr(dotPos);
    }
    return L"";
}

std::ostream& operator<<(std::ostream& os, const FileNode& node) {
    std::string indent(0, ' ');
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

    if (node.isVisible) {
        os << " [VISIBLE]";
    } else {
        os << " [HIDDEN]";
    }

    // Recursively print children
    for (const auto& child : node.children) {
        if (child) {
            os << "\n" << std::string(2, ' ');
            os << *child;
        }
    }

    return os;
}

}  // namespace FTree