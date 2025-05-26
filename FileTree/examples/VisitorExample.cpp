#include "visitor/fileTreeVisitorBase.h"
#include "FileTree/FileTree.h"
#include <iostream>
#include <typeinfo>

int main() {
    auto rootDir = std::make_unique<FileTree>();

    std::cout << "===== VARIANT-BASED VISITOR EXAMPLES =====\n\n";
    
    // Example 1: Collect all file extensions using the variant-based visitor
    {
        std::cout << "Example 1: Extension Collector Visitor\n";
        std::cout << "--------------------------------------\n";
        
        // Create the concrete visitor
        ExtensionCollectorVisitor collector;
        
        // Wrap it in the variant-based FileTreeVisitor
        FileTreeVisitor visitor(collector);
        visitor.traverse(rootDir->getRootNode()); 
        
        // Method 1: Get results directly from a copy of the original visitor
        std::cout << "Found file extensions (from original):\n";
        for (const auto& ext : collector.getExtensions()) {
            std::cout << "  " << ext << '\n';
        }
        
        // Method 2: Extract the visitor from the variant using getVisitor<T>()
        if (auto* extractedCollector = visitor.getVisitor<ExtensionCollectorVisitor>()) {
            std::cout << "Found file extensions (extracted from variant):\n";
            for (const auto& ext : extractedCollector->getExtensions()) {
                std::cout << "  " << ext << '\n';
            }
        }
        
        // Try to extract the wrong visitor type - will return nullptr
        if (visitor.getVisitor<FilteredTreeBuilderVisitor>()) {
            std::cout << "This should not print - wrong visitor type\n";
        } else {
            std::cout << "As expected, cannot extract FilteredTreeBuilderVisitor from this variant\n";
        }
        
        std::cout << std::endl;
    }
    
    // Example 2: Build a filtered tree with only C/C++ files
    {
        std::cout << "Example 2: Filtered Tree Builder Visitor\n";
        std::cout << "----------------------------------------\n";
        
        auto isCppFile = [](const FileNode* node) -> bool {
            if (node->type != FileType::FILE) return false;
            std::string ext = node->fullPath.extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), 
                [](unsigned char c) { return std::tolower(c); });
            return ext == ".cpp" || ext == ".c" || ext == ".h" || ext == ".hpp";
        };
        
        // Create the concrete visitor
        std::cout << "[STARTING FILTER TREE]" << "\n";
        FilteredTreeBuilderVisitor builder(isCppFile);
        
        // Wrap it in the variant-based FileTreeVisitor
        FileTreeVisitor visitor(builder); // Now works with copy constructor
        
        visitor.traverse(rootDir->getRootNode());
        std::cout << "[FINISHED FILTER TREE]" << "\n";
        auto* extractedbuild = visitor.getVisitor<FilteredTreeBuilderVisitor>();
        if (extractedbuild) {
            std::cout << "Successfully accessed the builder from the variant\n";
            std::unique_ptr<FileNode> filteredTree = extractedbuild->getFiltered();
            std::cout << "Filtered tree contains C/C++ files:\n";
            std::cout << *filteredTree.get() << "\n";
        }
        auto node = builder.getFiltered();
        std::cout << "Builder getter way" << "\n";
        std::cout << *node.get() << "\n";
        std::cout << "[Filtered Tree]" << "\n";
        // Method 1: Get results from the original builder
        // //auto filteredTree = builder.getFilteredTree();
        // std::cout << "Filtered tree contains C/C++ files:\n";
        // if (filteredTree) {
        //     for (auto &&child : filteredTree->children) {
        //         std::wcout << "  " << child->name << "\n";
        //     }
            
        //     if (filteredTree->children.empty()) {
        //         std::cout << "  (No C/C++ files found)\n";
        //     }
        // } else {
        //     std::cout << "  (Filtered tree is empty)\n";
        // }
        
        // Method 2: Extract the visitor from the variant
        if (auto* extractedBuilder = visitor.getVisitor<FilteredTreeBuilderVisitor>()) {
            std::cout << "Successfully accessed the builder from the variant\n";
        }
        
        if (!visitor.getVisitor<ExtensionCollectorVisitor>()) {
            std::cout << "As expected, cannot extract ExtensionCollectorVisitor from this variant\n";
        }
    }
    
    // Demonstrating type checking with the variant approach
    {
        std::cout << "\nExample 3: Demonstrating variant type checking\n";
        std::cout << "----------------------------------------------\n";
        
        ExtensionCollectorVisitor collector;
        FileTreeVisitor visitor(collector);
        visitor.traverse(rootDir->getRootNode()); 
        // Check visitor type using getVisitor<T>()
        if (visitor.getVisitor<ExtensionCollectorVisitor>()) {
            std::cout << "Visitor contains ExtensionCollectorVisitor\n";
            
            // Get extension count from the extracted visitor
            auto* extractedVisitor = visitor.getVisitor<ExtensionCollectorVisitor>();
            if (extractedVisitor) {
                std::cout << "Current extension count: " 
                          << extractedVisitor->getExtensions().size() << "\n";
            }
        }
        
        if (visitor.getVisitor<FilteredTreeBuilderVisitor>()) {
            std::cout << "Visitor contains FilteredTreeBuilderVisitor\n";
        } else {
            std::cout << "Visitor does not contain FilteredTreeBuilderVisitor\n";
        }
    }
    
    return 0;
}