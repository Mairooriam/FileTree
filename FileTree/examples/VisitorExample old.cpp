#include "visitor/fileTreeVisitor.h"
#include "FileTree/FileTree.h"
#include <iostream>

void traverseWithVisitor(FileNode* node, FileTreeVisitor& visitor) {
    if (!node) return;
    
    visitor.visit(node);
    
    // Visit all children first
    for (auto& child : node->children) {
        traverseWithVisitor(child.get(), visitor);
    }
    
    // Now we're done with this node level
    visitor.popNode();
}

int main() {
    // Create a sample file tree
    auto rootDir = std::make_unique<FileTree>();

    
    // Example 1: Collect all file extensions
    {
        ExtensionCollectorVisitor collector;
        FileTreeVisitor visitor(collector);
        traverseWithVisitor(rootDir->getRootNode(), visitor);
        
        // Now we need to extract the collector to get its state
        std::cout << "Found file extensions:\n";
        for (const auto& ext : collector.getExtensions()) {
            std::cout << ext << '\n';
        }
        std::cout << std::endl;
    }
    
    // Example 2: Build a filtered tree with only image files
    {
        auto isImageFile = [](const FileNode* node) -> bool {
            if (node->type != FileType::FILE) return false;
            std::string ext = node->fullPath.extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), 
                [](unsigned char c) { return std::tolower(c); });
            return ext == ".cpp" || ext == ".c";
        };
        
        FilteredTreeBuilderVisitor builder(isImageFile);
        FileTreeVisitor visitor(std::ref(builder));
        traverseWithVisitor(rootDir->getRootNode(), visitor);
        
        // Extract the filtered tree
        auto filteredTree = builder.getFilteredTree();
        std::cout << "Filtered tree contains " << filteredTree->children.size() << " .cpp files\n";
        for (auto &&child : filteredTree->children)
        {
            std::wcout <<  child->name << "\n";
        }
        
    }
    
    return 0;
}