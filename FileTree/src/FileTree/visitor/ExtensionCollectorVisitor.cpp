#include "ExtensionCollectorVisitor.h"

void ExtensionCollectorVisitor::operator()(FileNode* node) {
    if (node && node->type == FileType::FILE) {
        std::string ext = node->fullPath.extension().string();
        if (!ext.empty()) {
            std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) {
                return std::tolower(c);
            });
            m_extensions->insert(ext);
        }
    }
}