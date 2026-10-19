#pragma once
#include "taskflow/priority_strategy.hpp"
#include "taskflow/repository.hpp"
#include <functional>
#include <string>
#include <vector>

namespace taskflow {

using TaskListener = std::function<void(const std::string& event, const Task& task)>;

class TaskService {
public:
    TaskService(TaskRepositoryPtr repository, PriorityStrategyPtr priority_strategy);

    Task create_task(std::string title, std::string assignee, int priority,
                     std::string description = "", std::string tag = "general");
    bool assign(std::int64_t id, std::string assignee);
    bool transition(std::int64_t id, TaskStatus next);
    std::vector<Task> list_by_priority() const;
    std::optional<Task> find(std::int64_t id) const;

    // ЛР2: простой Observer
    void add_listener(TaskListener listener);

private:
    void validate(const std::string& title, const std::string& assignee,
                  int priority, const std::string& description) const;
    bool can_go(TaskStatus from, TaskStatus to) const;
    void notify(const std::string& event, const Task& task) const;

    TaskRepositoryPtr repository_;
    PriorityStrategyPtr strategy_;
    std::vector<TaskListener> listeners_;
};

} // namespace taskflow
