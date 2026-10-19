#pragma once
#include "taskflow/in_memory_task_repository.hpp"
#include "taskflow/sqlite_task_repository.hpp"
#include <memory>
#include <stdexcept>
#include <string>

namespace taskflow {

struct Config {
    std::string backend{"memory"};
    std::string db_file{"ramil.db"};
};

inline TaskRepositoryPtr make_repository(const Config& cfg) {
    if (cfg.backend == "memory") {
        return std::make_shared<InMemoryTaskRepository>();
    }
    if (cfg.backend == "sqlite") {
        return std::make_shared<SQLiteTaskRepository>(cfg.db_file);
    }
    throw std::invalid_argument("unknown backend: " + cfg.backend);
}

} // namespace taskflow
