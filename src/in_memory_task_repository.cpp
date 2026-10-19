#include "taskflow/in_memory_task_repository.hpp"
#include <algorithm>

namespace taskflow {

Task InMemoryTaskRepository::create(Task draft) {
    draft.id = next_id_++;
    tasks_[draft.id] = draft;
    return draft;
}

std::optional<Task> InMemoryTaskRepository::find_by_id(std::int64_t id) const {
    const auto it = tasks_.find(id);
    if (it == tasks_.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::vector<Task> InMemoryTaskRepository::list() const {
    std::vector<Task> result;
    result.reserve(tasks_.size());
    for (const auto& [id, task] : tasks_) {
        (void)id;
        result.push_back(task);
    }
    std::sort(result.begin(), result.end(), [](const Task& a, const Task& b) { return a.id < b.id; });
    return result;
}

bool InMemoryTaskRepository::update(const Task& task) {
    const auto it = tasks_.find(task.id);
    if (it == tasks_.end()) {
        return false;
    }
    it->second = task;
    return true;
}

} // namespace taskflow
