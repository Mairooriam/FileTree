
#pragma once
#include <functional>
#include <vector>

#include "commands/commands.h"
#include "FileTree/FileTree.h"

class ApplyFilterCmd : public Command {
    private:
        std::shared_ptr<FileTree> m_filetree;
        std::function<bool(const FileNode*)> m_filterPredicate;

    public:
        ApplyFilterCmd(std::shared_ptr<FileTree> _filetree, const std::vector<std::string>& _extensions);
        void execute() override;
        void undo() override;
        std::string getName() const override;
};


class ResetFilterCmd : public Command {
    private:
        std::shared_ptr<FileTree> m_filetree;
    public:
        ResetFilterCmd(std::shared_ptr<FileTree> _filetree);
        void execute() override;
        void undo() override;
        std::string getName() const override;
};
        
