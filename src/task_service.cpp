#include "taskflow/task_service.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>

namespace taskflow {

std::string to_string(TaskStatus status) {
    switch (status) {
        case TaskStatus::New: return "New";
        case TaskStatus::InProgress: return "InProgress";
        case TaskStatus::Done: return "Done";
        case TaskStatus::Cancelled: return "Cancelled";
    }
    return "Unknown";
}

TaskStatus status_from_string(const std::string& value) {
    if (value == "New") return TaskStatus::New;
    if (value == "InProgress") return TaskStatus::InProgress;
    if (value == "Done") return TaskStatus::Done;
    if (value == "Cancelled") return TaskStatus::Cancelled;
    throw std::invalid_argument("status?");
}

TaskService::TaskService(TaskRepositoryPtr repository, PriorityStrategyPtr priority_strategy)
    : repository_(std::move(repository)), strategy_(std::move(priority_strategy)) {
    if (!repository_ || !strategy_) {
        throw std::invalid_argument("null dependency");
    }
}

void TaskService::add_listener(TaskListener listener) {
    if (listener) {
        listeners_.push_back(std::move(listener));
    }
}

void TaskService::notify(const std::string& event, const Task& task) const {
    for (const auto& listener : listeners_) {
        listener(event, task);
    }
}

Task TaskService::create_task(std::string title, std::string assignee, int priority,
                              std::string description, std::string tag) {
    validate(title, assignee, priority, description);
    Task draft;
    draft.title = std::move(title);
    draft.assignee = std::move(assignee);
    draft.priority = priority;
    draft.description = std::move(description);
    draft.tag = std::move(tag);
    draft.status = TaskStatus::New;
    auto created = repository_->create(std::move(draft));
    notify("TaskCreated", created);
    return created;
}

bool TaskService::assign(std::int64_t id, std::string assignee) {
    if (assignee.empty()) {
        throw std::invalid_argument("empty assignee");
    }
    auto task = repository_->find_by_id(id);
    if (!task || is_closed(task->status)) {
        return false;
    }
    task->assignee = std::move(assignee);
    return repository_->update(*task);
}

bool TaskService::transition(std::int64_t id, TaskStatus next) {
    auto task = repository_->find_by_id(id);
    if (!task || !can_go(task->status, next)) {
        return false;
    }
    task->status = next;
    return repository_->update(*task);
}

std::vector<Task> TaskService::list_by_priority() const {
    auto tasks = repository_->list();
    std::sort(tasks.begin(), tasks.end(), [&](const Task& a, const Task& b) {
        const int sa = strategy_->score(a);
        const int sb = strategy_->score(b);
        return sa == sb ? a.id < b.id : sa < sb;
    });
    return tasks;
}

std::optional<Task> TaskService::find(std::int64_t id) const {
    return repository_->find_by_id(id);
}

void TaskService::validate(const std::string& title, const std::string& assignee,
                           int priority, const std::string& description) const {
    if (title.empty()) throw std::invalid_argument("empty title");
    if (assignee.empty()) throw std::invalid_argument("empty assignee");
    if (priority < 1 || priority > 5) throw std::invalid_argument("bad priority");
    if (description.size() > 1000) throw std::invalid_argument("description > 1000");
}

bool TaskService::can_go(TaskStatus from, TaskStatus to) const {
    if (from == to) return true;
    if (is_closed(from)) return false;
    if (from == TaskStatus::New) {
        return to == TaskStatus::InProgress || to == TaskStatus::Cancelled;
    }
    if (from == TaskStatus::InProgress) {
        return to == TaskStatus::Done || to == TaskStatus::Cancelled;
    }
    return false;
}

} // namespace taskflow
