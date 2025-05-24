#pragma once
#include "vector"
#include <memory>
#include <string>

class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual std::string getName() const = 0; 
    virtual void undo() = 0;
};
class CommandManager {
private:
    std::vector<std::unique_ptr<Command>> history;
    size_t currentIndex = 0;
    
    CommandManager() = default;
    
    CommandManager(const CommandManager&) = delete;
    CommandManager& operator=(const CommandManager&) = delete;
    CommandManager(CommandManager&&) = delete;
    CommandManager& operator=(CommandManager&&) = delete;
    
public:
    static CommandManager& instance() {
        static CommandManager instance;
        return instance;
    }

    void execute(std::unique_ptr<Command> command);
    bool canUndo() const { return currentIndex > 0; }
    bool canRedo() const { return currentIndex < history.size(); }
    void undo();
    void redo();
 
    // New methods to support UI
    size_t getCurrentIndex() const { return currentIndex; }
    size_t getHistorySize() const { return history.size(); }
    std::string getCommandName(size_t index) const {
        if (index < history.size()) {
            return history[index]->getName();
        }
        return "";
    }

    friend std::ostream& operator<<(std::ostream& os, const CommandManager& manager);
};
       
