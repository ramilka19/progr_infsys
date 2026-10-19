#include "taskflow/priority_strategy.hpp"

namespace taskflow {

int DefaultPriorityStrategy::score(const Task& task) const {
    return 10 - task.priority;
}

int ClosedLastPriorityStrategy::score(const Task& task) const {
    return is_closed(task.status) ? 1000 : 10 - task.priority;
}

int AssigneePriorityStrategy::score(const Task& task) const {
    if (is_closed(task.status)) {
        return 1500;
    }
    if (task.assignee.empty()) {
        return 900; // без исполнителя ниже
    }
    return 10 - task.priority;
}

} // namespace taskflow
