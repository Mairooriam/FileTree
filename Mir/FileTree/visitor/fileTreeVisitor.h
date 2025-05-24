#pragma once
#include "FileNode.h"
#include <set>
#include <algorithm>
#include <functional>
#include <stack>
#include <memory>
#include <variant>
#include <iostream>
// Forward declarations of visitor types
// Extension collector visitor
struct ExtensionCollectorVisitor {
    std::shared_ptr<std::set<std::string>> m_extensions;
    
    ExtensionCollectorVisitor() 
        : m_extensions(std::make_shared<std::set<std::string>>()) {}
    
    void operator()(FileNode* node) {
        if (node && node->type == FileType::FILE) {
            std::string ext = node->fullPath.extension().string();
            if (!ext.empty()) {
                std::transform(ext.begin(), ext.end(), ext.begin(), 
                    [](unsigned char c) { return std::tolower(c); });
                m_extensions->insert(ext);
            }
        }
    }
    
    const std::set<std::string>& getExtensions() const { 
        return *m_extensions; 
    }
};

// Filter tree builder visitor
struct FilteredTreeBuilderVisitor {
    std::unique_ptr<FileNode> m_filteredRoot;
    std::function<bool(const FileNode*)> m_filterPredicate;
    FileNode* m_currentFilteredNode{nullptr};
    std::stack<FileNode*> m_nodeStack;

    std::unique_ptr<FileNode> getFiltered() {
        // Create a deep copy instead of moving
        if (m_filteredRoot) {
            return std::make_unique<FileNode>(*m_filteredRoot);
        }
        return nullptr;
    }

    explicit FilteredTreeBuilderVisitor(std::function<bool(const FileNode*)> predicate)
        : m_filterPredicate(std::move(predicate)) {
        std::cout << "FilteredTreeBuilderVisitor created with predicate\n";
    }
    
    // Add copy constructor
    FilteredTreeBuilderVisitor(const FilteredTreeBuilderVisitor& other)
        : m_filterPredicate(other.m_filterPredicate),
          m_currentFilteredNode(nullptr) {
        // Deep copy the filtered root if it exists
        if (other.m_filteredRoot) {
            m_filteredRoot = std::make_unique<FileNode>(*other.m_filteredRoot);
        }
        std::cout << "FilteredTreeBuilderVisitor copy constructed\n";
    }
    
    // Add move constructor
    FilteredTreeBuilderVisitor(FilteredTreeBuilderVisitor&& other) noexcept
        : m_filteredRoot(std::move(other.m_filteredRoot)),
          m_filterPredicate(std::move(other.m_filterPredicate)),
          m_currentFilteredNode(other.m_currentFilteredNode),
          m_nodeStack(std::move(other.m_nodeStack)) {
        other.m_currentFilteredNode = nullptr;
        std::cout << "FilteredTreeBuilderVisitor move constructed\n";
    }
    
    void operator()(FileNode* node) {
        std::wcout << "Visiting node: " << node->name << "\n";
        if(!node) return;
        
        bool includeNode = m_filterPredicate(node);
        bool isDirectory = node->type != FileType::FILE;
        
        // For directories, we'll include them if they match the predicate directly
        // or if we need them as a parent for matching children
    if (includeNode || isDirectory) {
        // First node - create the filtered root
        if (!m_filteredRoot) {
            m_filteredRoot = std::make_unique<FileNode>(*node, false);
            m_currentFilteredNode = m_filteredRoot.get();
        } 
        // Child node that passes the filter
        else if (m_currentFilteredNode) {
            // Create a copy of the node as a child of current filtered node
            auto newNode = std::make_unique<FileNode>(*node);
            
            // Save the new node pointer before moving ownership
            FileNode* addedNode = newNode.get();
            
            // Add as child to current filtered node
            m_currentFilteredNode->children.push_back(std::move(newNode));
            
            // Push current filtered node to stack before moving to new node
            m_nodeStack.push(m_currentFilteredNode);
            
            // Update current to the newly added node
            m_currentFilteredNode = addedNode;
        }
    } else {
        // If node didn't pass the filter, still push current node
        // so the stack depth matches tree depth
        m_nodeStack.push(m_currentFilteredNode);
        m_currentFilteredNode = nullptr; // No matching node at this level
    }
        

        std::cout << "AFTEr visit m_filteredRoot:" << "\n";
        std::cout << *m_filteredRoot << "\n";
    }

    void popNode() {
        // We've finished processing a node's children, go back to parent
        if (!m_nodeStack.empty()) {
            m_currentFilteredNode = m_nodeStack.top();
            m_nodeStack.pop();
        } else {
            m_currentFilteredNode = nullptr;
        }
    }
    

};

// Variant-based visitor interface
class FileTreeVisitor {
private:
    // Define the variant that can hold any of your visitor types
    using VisitorVariant = std::variant<
        ExtensionCollectorVisitor,
        FilteredTreeBuilderVisitor
        // Add more visitor types here as needed
    >;
    
    VisitorVariant m_visitor;
    
public:
    // Constructor that accepts any of the supported visitor types
    template<typename T, 
             typename = std::enable_if_t<std::is_constructible_v<VisitorVariant, T>>>
    explicit FileTreeVisitor(T visitor) 
        : m_visitor(std::move(visitor)) {}
    
    // Copy operations use the variant's implicit copying
    FileTreeVisitor(const FileTreeVisitor&) = default;
    FileTreeVisitor& operator=(const FileTreeVisitor&) = default;
    
    // Move operations
    FileTreeVisitor(FileTreeVisitor&&) = default;
    FileTreeVisitor& operator=(FileTreeVisitor&&) = default;
    
    // Visit method using std::visit to dispatch to the correct visitor
    void visit(FileNode* node) {
        std::visit([node](auto& visitor) {
            visitor(node);
        }, m_visitor);
    }
    
    // Pop node method - checks if the visitor has a popNode method
    void popNode() {
        std::visit([](auto& visitor) {
            if constexpr (requires { visitor.popNode(); }) {
                visitor.popNode();
            }
        }, m_visitor);
    }
    void traverse(FileNode* node) {
        if (!node) return;
        
        visit(node);
        
        // Visit all children
        for (auto& child : node->children) {
            traverse(child.get());
        }
        popNode();
        
    }
    // Helper to access the underlying visitor
    template<typename T>
    T* getVisitor() {
        return std::get_if<T>(&m_visitor);
    }
    
    template<typename T>
    const T* getVisitor() const {
        return std::get_if<T>(&m_visitor);
    }
};

