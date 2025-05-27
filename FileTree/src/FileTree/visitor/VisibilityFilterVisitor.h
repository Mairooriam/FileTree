#pragma once
#include <algorithm>
#include <functional>
#include <iostream>
#include <unordered_set>
#include "FileNode.h"

namespace FTree {
class VisibilityFilterVisitor {
public:
    using FilterPredicate = std::function<bool(const FileNode*)>;

private:
    FilterPredicate m_predicate;

public:
    // Constructor with custom predicate
    explicit VisibilityFilterVisitor(FilterPredicate predicate = nullptr);

    void operator()(FileNode* node);
};
}  // namespace FTree