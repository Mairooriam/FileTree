#include "VisibilityFilterVisitor.h"

namespace FTree {

    VisibilityFilterVisitor::VisibilityFilterVisitor(FilterPredicate predicate)
        : m_predicate(predicate) {}

    void VisibilityFilterVisitor::operator()(FileNode* node) {
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
