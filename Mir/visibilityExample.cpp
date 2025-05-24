#include <iostream>
#include <algorithm>
#include <string>
#include "visitor/fileTreeVisitorBase.h"
#include "FileTree/FileTree.h"
#include <iostream>
#include <typeinfo>
#include <queue>
void expandAllNodes(FileTree& tree, FileNode* node) {
    if (!node) return;
    
    std::cout << "Expanding node: " << std::string(node->name.begin(), node->name.end()) << std::endl;

    
    // Use BFS to expand all nodes
}

int main() {
    // Create a file tree
    auto fileTree = std::make_unique<FileTree>();
    // Expand all nodes for testing purposes
    std::cout << "===== EXPANDING ALL NODES FOR TESTING =====\n\n";
    for (auto &&child : fileTree->getRootNode()->children)
    {
        fileTree->expandNode(child.get());
        
    }
    FileNode* rootNode = fileTree->getRootNode();
    std::cout << "===== EXPANSION COMPLETE =====\n\n";
    
    std::cout << "===== VISIBILITY FILTER VISITOR EXAMPLES =====\n\n";
    
    // Example 1: Filter to show only files with specific extension
    {
        std::cout << "Example 1: Show only .cpp and .h files\n";
        std::cout << "--------------------------------------\n";
        
        auto isCppOrHeaderFile = [](const FileNode* node) -> bool {
            if (node->type != FileType::FILE) return false;
            
            std::filesystem::path filePath(node->fullPath);
            std::string ext = filePath.extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), 
                [](unsigned char c) { return std::tolower(c); });
                
            return ext == ".cpp" || ext == ".h" || ext == ".hpp";
        };
        
        // Create the concrete visitor
        std::cout << "[STARTING FILTER - C++ FILES]" << "\n";
        VisibilityFilterVisitor filterVisitor(isCppOrHeaderFile);
        
        // Wrap it in the variant-based FileTreeVisitor
        FileTreeVisitor visitor(filterVisitor);
        
        visitor.traverse(rootNode);
        std::cout << "[FINISHED FILTER - C++ FILES]" << "\n";
        
        // Print the tree showing visibility status
        std::cout << "Tree with C++ files visible:\n";
        std::cout << *rootNode << std::endl;
    }
    
    // Example 2: Filter by file size (only show files larger than 1MB)
    {
        std::cout << "Example 2: Show only files larger than 1MB\n";
        std::cout << "----------------------------------------\n";
        
        // Reset visibility flags
        auto resetVisibility = [](const FileNode* node) -> bool {
            const_cast<FileNode*>(node)->isVisible = true;
            return true; // Continue traversal
        };
        
        // Create reset visitor and wrap it
        VisibilityFilterVisitor resetFilterVisitor(resetVisibility);
        FileTreeVisitor resetVisitor(resetFilterVisitor);
        resetVisitor.traverse(rootNode);
        
        // Define size filter predicate
        auto isLargeFile = [](const FileNode* node) -> bool {
            if (node->type != FileType::FILE) return false;
            return node->size > 1024 * 1024; // > 1MB
        };
        
        // Create the concrete visitor
        std::cout << "[STARTING FILTER - LARGE FILES]" << "\n";
        VisibilityFilterVisitor sizeFilterVisitor(isLargeFile);
        
        // Wrap it in the variant-based FileTreeVisitor
        FileTreeVisitor visitor(sizeFilterVisitor);
        
        visitor.traverse(rootNode);
        std::cout << "[FINISHED FILTER - LARGE FILES]" << "\n";
        
        // Print the tree showing visibility status
        std::cout << "Tree with large files visible:\n";
        std::cout << *rootNode << std::endl;
    }
    
    // Example 3: Filter by name pattern
    {
        std::cout << "Example 3: Show only files containing 'test' in their name\n";
        std::cout << "------------------------------------------------------\n";
        
        // Reset visibility flags
        auto resetVisibility = [](const FileNode* node) -> bool {
            const_cast<FileNode*>(node)->isVisible = true;
            return true;
        };
        
        // Create reset visitor and wrap it
        VisibilityFilterVisitor resetFilterVisitor(resetVisibility);
        FileTreeVisitor resetVisitor(resetFilterVisitor);
        resetVisitor.traverse(rootNode);
        
        // Define name filter predicate
        auto isTestFile = [](const FileNode* node) -> bool {
            if (node->type != FileType::FILE) return false;
            
            // Convert wide string to regular string for case-insensitive comparison
            std::wstring name = node->name;
            std::string nameStr(name.begin(), name.end());
            std::transform(nameStr.begin(), nameStr.end(), nameStr.begin(),
                [](unsigned char c) { return std::tolower(c); });
                
            return nameStr.find("test") != std::string::npos;
        };
        
        // Create the concrete visitor
        std::cout << "[STARTING FILTER - TEST FILES]" << "\n";
        VisibilityFilterVisitor nameFilterVisitor(isTestFile);
        
        // Wrap it in the variant-based FileTreeVisitor
        FileTreeVisitor visitor(nameFilterVisitor);
        
        visitor.traverse(rootNode);
        std::cout << "[FINISHED FILTER - TEST FILES]" << "\n";
        
        // Print the tree showing visibility status
        std::cout << "Tree with test files visible:\n";
        std::cout << *rootNode << std::endl;
    }
    
    return 0;
}