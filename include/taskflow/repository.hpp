#pragma once
#include "taskflow/task.hpp"
#include <memory>
#include <optional>
#include <vector>

namespace taskflow {

class ITaskRepository {
public:
    virtual ~ITaskRepository() = default;
    virtual Task create(Task draft) = 0;
    virtual std::optional<Task> find_by_id(std::int64_t id) const = 0;
    virtual std::vector<Task> list() const = 0;
    virtual bool update(const Task& task) = 0;
};

using TaskRepositoryPtr = std::shared_ptr<ITaskRepository>;

} // namespace taskflow
