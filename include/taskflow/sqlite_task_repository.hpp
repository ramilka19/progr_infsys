#pragma once
#include "taskflow/repository.hpp"
#include <stdexcept>
#include <string>

namespace taskflow {

class SQLiteTaskRepository final : public ITaskRepository {
public:
    explicit SQLiteTaskRepository(std::string file) : file_(std::move(file)) {}

    Task create(Task) override { boom(); }
    std::optional<Task> find_by_id(std::int64_t) const override { boom(); }
    std::vector<Task> list() const override { boom(); }
    bool update(const Task&) override { boom(); }

    const std::string& file() const { return file_; }

private:
    std::string file_;
    [[noreturn]] void boom() const {
        throw std::logic_error("sqlite stub only, file=" + file_);
    }
};

} // namespace taskflow
