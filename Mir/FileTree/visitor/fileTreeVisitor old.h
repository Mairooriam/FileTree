#pragma once
#include "FileNode.h"
#include <set>
#include <algorithm>
#include <functional>
#include <stack>
#include <memory>

// Value-based visitor interface using type erasure
class FileTreeVisitor {
private:
    struct VisitorConcept {
        virtual ~VisitorConcept() = default;
        virtual void visit(FileNode* node) = 0;
        virtual void popNode() {};  // Optional operation
        virtual std::unique_ptr<VisitorConcept> clone() const = 0;
    };
    
    template<typename T>
    struct VisitorModel : public VisitorConcept {
        T visitor;
        
        explicit VisitorModel(T v) : visitor(std::move(v)) {}
        
        void visit(FileNode* node) override {
            visitor(node);
        }
        
        void popNode() override {
            if constexpr (requires(T t) { t.popNode(); }) {
                visitor.popNode();
            }
        }
        
        std::unique_ptr<VisitorConcept> clone() const override {
            return std::make_unique<VisitorModel<T>>(visitor);
        }
    };
    
    std::unique_ptr<VisitorConcept> m_impl;
    
public:
    template<typename T>
    explicit FileTreeVisitor(T visitor) 
        : m_impl(std::make_unique<VisitorModel<T>>(std::move(visitor))) {}
    
    FileTreeVisitor(const FileTreeVisitor& other)
        : m_impl(other.m_impl->clone()) {}
        
    FileTreeVisitor& operator=(const FileTreeVisitor& other) {
        m_impl = other.m_impl->clone();
        return *this;
    }
    
    FileTreeVisitor(FileTreeVisitor&&) = default;
    FileTreeVisitor& operator=(FileTreeVisitor&&) = default;
    
    void visit(FileNode* node) {
        m_impl->visit(node);
    }
    
    void popNode() {
        m_impl->popNode();
    }
};

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
    
    explicit FilteredTreeBuilderVisitor(std::function<bool(const FileNode*)> predicate)
        : m_filterPredicate(std::move(predicate)) {
    }
    
    void operator()(FileNode* node) {
        if (!node) return;
        
        // For the root node, initialize the filtered tree
        if (m_filteredRoot == nullptr) {
            m_filteredRoot = std::make_unique<FileNode>(*node); // Copy root node
            m_filteredRoot->children.clear(); // Clear children to rebuild with only filtered ones
            m_currentFilteredNode = m_filteredRoot.get();
            m_nodeStack.push(m_currentFilteredNode);
        }
        
        // Apply filter to children
        for (auto& childNode : node->children) {
            if (childNode->type == FileType::DIR || m_filterPredicate(childNode.get())) {
                auto filteredChild = std::make_unique<FileNode>(*childNode);
                filteredChild->children.clear();
                
                FileNode* childPtr = filteredChild.get();
                m_currentFilteredNode->children.push_back(std::move(filteredChild));
                m_nodeStack.push(m_currentFilteredNode);
                // If it's a directory, we'll need to visit its children too
                // if (childNode->type == FileType::DIR) {
                //     m_currentFilteredNode = childPtr;
                //     m_nodeStack.push(m_currentFilteredNode);
                //     // Recursive visit happens in the FileTree::traverseWithVisitor method
                // }
            }
        }
    }
    
    void popNode() {
        if (!m_nodeStack.empty()) {
            m_nodeStack.pop();
            m_currentFilteredNode = m_nodeStack.empty() ? nullptr : m_nodeStack.top();
        }
    }
    
    std::unique_ptr<FileNode> getFilteredTree() {
        return std::move(m_filteredRoot);
    }
};