#include <iostream>

#include "FilteredTreeBuilderVisitor.h"

std::unique_ptr<FileNode> FilteredTreeBuilderVisitor::getFiltered() {
    if (m_filteredRoot) {
        return std::make_unique<FileNode>(*m_filteredRoot);
    }
    return nullptr;
}

FilteredTreeBuilderVisitor::FilteredTreeBuilderVisitor(const FilteredTreeBuilderVisitor& other)
    : m_filterPredicate(other.m_filterPredicate), m_currentFilteredNode(nullptr) {
    // Deep copy the filtered root if it exists
    if (other.m_filteredRoot) {
        m_filteredRoot = std::make_unique<FileNode>(*other.m_filteredRoot);
    }
    std::cout << "FilteredTreeBuilderVisitor copy constructed\n";
}

FilteredTreeBuilderVisitor::FilteredTreeBuilderVisitor(FilteredTreeBuilderVisitor&& other) noexcept
    : m_filteredRoot(std::move(other.m_filteredRoot)),
      m_filterPredicate(std::move(other.m_filterPredicate)),
      m_currentFilteredNode(other.m_currentFilteredNode),
      m_nodeStack(std::move(other.m_nodeStack)) {
    other.m_currentFilteredNode = nullptr;
    std::cout << "FilteredTreeBuilderVisitor move constructed\n";
}

void FilteredTreeBuilderVisitor::operator()(FileNode* node) {
    std::wcout << "Visiting node: " << node->name << "\n";
    if (!node)
        return;

    bool includeNode = m_filterPredicate(node);
    bool isDirectory = node->type != FileType::FILE;

    if (includeNode || isDirectory) {
        if (!m_filteredRoot) {
            m_filteredRoot = std::make_unique<FileNode>(*node, false);
            m_currentFilteredNode = m_filteredRoot.get();
        } else if (m_currentFilteredNode) {
            auto newNode = std::make_unique<FileNode>(*node);
            FileNode* addedNode = newNode.get();
            m_currentFilteredNode->children.push_back(std::move(newNode));
            m_nodeStack.push(m_currentFilteredNode);
            m_currentFilteredNode = addedNode;
        }
    } else {
        m_nodeStack.push(m_currentFilteredNode);
        m_currentFilteredNode = nullptr;
    }
    std::cout << "AFTEr visit m_filteredRoot:" << "\n";
    std::cout << *m_filteredRoot << "\n";
}

void FilteredTreeBuilderVisitor::popNode() {
    if (!m_nodeStack.empty()) {
        m_currentFilteredNode = m_nodeStack.top();
        m_nodeStack.pop();
    } else {
        m_currentFilteredNode = nullptr;
    }
}