#include <iostream>


#include "commands/commands.h"

CommandManager& CommandManager::getInstance() {
    static CommandManager instance;
    return instance;
}

void CommandManager::execute(std::unique_ptr<Command> command) {
    if (currentIndex < history.size()) {
        history.resize(currentIndex);
    }
    command->execute();
    history.push_back(std::move(command));
    currentIndex++;
    std::cout << "Executed: " << history.back()->getName() << std::endl;
}
void CommandManager::undo(){



    if (!canUndo()) {
        std::cout << "Nothing to undo" << std::endl;
        return;
    }
    currentIndex--;
    history[currentIndex]->undo();
    std::cout << "Undone: " << history[currentIndex]->getName() << std::endl;
}


void CommandManager::redo() {
    if (!canRedo()) {
        std::cout << "Nothing to redo" << std::endl;
        return;
    }
    history[currentIndex]->execute();
    std::cout << "Redone: " << history[currentIndex]->getName() << std::endl;
    currentIndex++;
}

std::ostream& operator<<(std::ostream& os, const CommandManager& manager) {
    os << "Command History:" << std::endl;
    for (size_t i = 0; i < manager.history.size(); i++) {
        os << i << ": " << manager.history[i]->getName() 
           << (i == manager.currentIndex - 1 ? " (current)" : "") 
           << std::endl;
    }
    return os;
}