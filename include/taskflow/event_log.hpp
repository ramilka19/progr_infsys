#pragma once
#include "taskflow/task.hpp"
#include <string>
#include <vector>

namespace taskflow {

// ЛР2: журнал как список строк (другой стиль, чем EventJournal со структурой)
class EventLog {
public:
    void push(const std::string& event, const Task& task) {
        lines_.push_back(event + " #" + std::to_string(task.id) + " " + task.title);
    }

    const std::vector<std::string>& lines() const { return lines_; }
    std::size_t size() const { return lines_.size(); }

private:
    std::vector<std::string> lines_;
};

} // namespace taskflow
