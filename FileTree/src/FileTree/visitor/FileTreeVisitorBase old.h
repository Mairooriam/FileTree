#pragma once
#include <memory>
#include <variant>
#include "FileNode.h"


#include "visitor/ExtensionCollectorVisitor.h"
#include "visitor/FilteredTreeBuilderVisitor.h"

class FileTreeVisitor {
private:
    using VisitorVariant = std::variant<ExtensionCollectorVisitor, FilteredTreeBuilderVisitor>;

    VisitorVariant m_visitor;

public:
    template<typename T, typename = std::enable_if_t<std::is_constructible_v<VisitorVariant, T>>>
    explicit FileTreeVisitor(T&& visitor) : m_visitor(std::forward<T>(visitor)) {
        // Explicitly move the visitor into the variant
    }

    // No copy operations - prevent accidental duplication
    FileTreeVisitor(const FileTreeVisitor&) = default;
    FileTreeVisitor& operator=(const FileTreeVisitor&) = default;

    // template<typename T, typename = std::enable_if_t<std::is_constructible_v<VisitorVariant, T>>>
    // explicit FileTreeVisitor(T visitor) : m_visitor(std::move(visitor)) {}

    // FileTreeVisitor(const FileTreeVisitor&) = default;
    // FileTreeVisitor& operator=(const FileTreeVisitor&) = default;

    FileTreeVisitor(FileTreeVisitor&&) = default;
    FileTreeVisitor& operator=(FileTreeVisitor&&) = default;
 
    void visit(FileNode* node) {
        std::visit([node](auto& visitor) { visitor(node); }, m_visitor);
    }

    void popNode() {
        std::visit(
            [](auto& visitor) {
                if constexpr (requires { visitor.popNode(); }) {
                    visitor.popNode();
                }
            },
            m_visitor);
    }
    void traverse(FileNode* node) {
        if (!node)
            return;

        visit(node);

        for (auto& child : node->children) {
            traverse(child.get());
        }
        popNode();
    }
    template<typename T>
    T* getVisitor() {
        return std::get_if<T>(&m_visitor);
    }

    template<typename T>
    const T* getVisitor() const {
        return std::get_if<T>(&m_visitor);
    }
};
