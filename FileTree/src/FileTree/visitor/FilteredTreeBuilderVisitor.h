#include <memory>
#include <functional>
#include <stack>
#include <iostream>

#include "FileNode.h"

struct FilteredTreeBuilderVisitor {
    std::unique_ptr<FileNode> m_filteredRoot;
    std::function<bool(const FileNode*)> m_filterPredicate;
    FileNode* m_currentFilteredNode{nullptr};
    std::stack<FileNode*> m_nodeStack;

    std::unique_ptr<FileNode> getFiltered(); 

    explicit FilteredTreeBuilderVisitor(std::function<bool(const FileNode*)> predicate)
        : m_filterPredicate(std::move(predicate)) {
        std::cout << "FilteredTreeBuilderVisitor created with predicate\n";
    }
    
    FilteredTreeBuilderVisitor(const FilteredTreeBuilderVisitor& other);
    FilteredTreeBuilderVisitor(FilteredTreeBuilderVisitor&& other) noexcept;
    void operator()(FileNode* node);
    void popNode(); 
};