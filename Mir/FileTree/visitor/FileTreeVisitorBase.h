#pragma once
#include <memory>
#include <variant>
#include "FileNode.h"


#include "visitor/ExtensionCollectorVisitor.h"
#include "visitor/FilteredTreeBuilderVisitor.h"
#include "visitor/VisibilityFilterVisitor.h"

class FileTreeVisitor {
private:
    using VisitorVariant = std::variant<
        std::reference_wrapper<ExtensionCollectorVisitor>, 
        std::reference_wrapper<FilteredTreeBuilderVisitor>,
        std::reference_wrapper<VisibilityFilterVisitor> 
    >;

    VisitorVariant m_visitor;

public:
    // Keep clean syntax with references instead of move semantics
    template<typename T, typename = std::enable_if_t<std::is_constructible_v<VisitorVariant, std::reference_wrapper<T>>>>
    explicit FileTreeVisitor(T& visitor) : m_visitor(std::ref(visitor)) {}

    // Copy operations still allowed
    FileTreeVisitor(const FileTreeVisitor&) = default;
    FileTreeVisitor& operator=(const FileTreeVisitor&) = default;

    // Move operations allowed
    FileTreeVisitor(FileTreeVisitor&&) = default;
    FileTreeVisitor& operator=(FileTreeVisitor&&) = default;
 
    void visit(FileNode* node) {
        std::visit([node](auto& visitor) { visitor.get()(node); }, m_visitor);
    }

    void popNode() {
        std::visit(
            [](auto& visitor) {
                auto& v = visitor.get();
                if constexpr (requires { v.popNode(); }) {
                    v.popNode();
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
        auto* ref = std::get_if<std::reference_wrapper<T>>(&m_visitor);
        return ref ? &(ref->get()) : nullptr;
    }

    template<typename T>
    const T* getVisitor() const {
        auto* ref = std::get_if<std::reference_wrapper<T>>(&m_visitor);
        return ref ? &(ref->get()) : nullptr;
    }
};
