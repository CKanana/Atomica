#include "core/StageManager.h"

#include <utility>

void StageManager::add(std::unique_ptr<Stage> stage) {
    stages_.push_back(std::move(stage));
    if (stages_.size() == 1) stages_[0]->onEnter();
}

void StageManager::switchTo(std::size_t index) {
    if (index >= stages_.size() || index == current_) return;
    stages_[current_]->onExit();
    current_ = index;
    stages_[current_]->onEnter();
}

void StageManager::next() {
    if (!stages_.empty()) switchTo((current_ + 1) % stages_.size());
}

void StageManager::previous() {
    if (!stages_.empty()) switchTo((current_ + stages_.size() - 1) % stages_.size());
}

Stage* StageManager::current() {
    return stages_.empty() ? nullptr : stages_[current_].get();
}