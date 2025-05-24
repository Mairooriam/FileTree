#pragma once
#include <memory>
#include <set>
#include <string>

#include "FileNode.h"

struct ExtensionCollectorVisitor {
    std::shared_ptr<std::set<std::string>> m_extensions;
    
    ExtensionCollectorVisitor() 
        : m_extensions(std::make_shared<std::set<std::string>>()) {}
    void operator()(FileNode* node); 
    const std::set<std::string>& getExtensions() const { return *m_extensions; }
};