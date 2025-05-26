#pragma once
#include "FileNode.h"
#include <functional>
#include <unordered_set>
#include <algorithm>
#include <iostream>
class VisibilityFilterVisitor {
public:
    using FilterPredicate = std::function<bool(const FileNode*)>;

private:
    FilterPredicate m_predicate;

public:
    // Constructor with custom predicate
    explicit VisibilityFilterVisitor(FilterPredicate predicate = nullptr)
        : m_predicate(predicate) {}
        
    void operator()(FileNode* node) {
        std::wcout << "Visiting node: " << node->name << "\n";
        if (!node)
            return;

        bool includeNode = m_predicate(node);
        bool isDirectory = node->type != FileType::FILE;

        if (includeNode || isDirectory) {
                node->isVisible = true;
        } else {
            node->isVisible = false;
        }

    }
};