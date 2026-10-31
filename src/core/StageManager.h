#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "core/Stage.h"

class StageManager {
public:
    void add(std::unique_ptr<Stage> stage);
    void switchTo(std::size_t index);
    void next();
    void previous();

    Stage* current();
    std::size_t currentIndex() const { return current_; }
    std::size_t count() const { return stages_.size(); }

private:
    std::vector<std::unique_ptr<Stage>> stages_;
    std::size_t current_ = 0;
};