#pragma once
#include "taskflow/repository.hpp"
#include <unordered_map>

namespace taskflow {

class InMemoryTaskRepository final : public ITaskRepository {
public:
    Task create(Task draft) override;
    std::optional<Task> find_by_id(std::int64_t id) const override;
    std::vector<Task> list() const override;
    bool update(const Task& task) override;

private:
    std::int64_t next_id_{1};
    std::unordered_map<std::int64_t, Task> tasks_;
};

} // namespace taskflow
