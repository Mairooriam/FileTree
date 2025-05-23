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
    
    std::unique_ptr<FileNode> getFiltered(){ return std::move(m_filteredRoot); }

    explicit FilteredTreeBuilderVisitor(std::function<bool(const FileNode*)> predicate)
        : m_filterPredicate(std::move(predicate)) {
    }
    
    // Add copy constructor
    FilteredTreeBuilderVisitor(const FilteredTreeBuilderVisitor& other)
        : m_filterPredicate(other.m_filterPredicate),
          m_currentFilteredNode(nullptr) {
        // Deep copy the filtered root if it exists
        if (other.m_filteredRoot) {
            m_filteredRoot = std::make_unique<FileNode>(*other.m_filteredRoot);
        }
    }
    
    // Add move constructor
    FilteredTreeBuilderVisitor(FilteredTreeBuilderVisitor&& other) noexcept
        : m_filteredRoot(std::move(other.m_filteredRoot)),
          m_filterPredicate(std::move(other.m_filterPredicate)),
          m_currentFilteredNode(other.m_currentFilteredNode),
          m_nodeStack(std::move(other.m_nodeStack)) {
        other.m_currentFilteredNode = nullptr;
    }
    
    void operator()(FileNode* node) {
        if (m_filteredRoot == nullptr)
        {
            m_filteredRoot = std::make_unique<FileNode>();
            std::cout << "initial node" << "\n";
            std::cout << *node << "\n";
        } else {
            if (m_filterPredicate(node) || node->type == FileType::DIR)
            {
                std::wcout << "[ADDED TO ROOT]" <<node->getExtension() << "\n";
                m_filteredRoot->children.push_back(std::make_unique<FileNode>(*node));
            }
            
        }
        
        
        
    }
    
    void popNode() {
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

