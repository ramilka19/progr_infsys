#include "taskflow/event_log.hpp"
#include "taskflow/in_memory_task_repository.hpp"
#include "taskflow/priority_strategy.hpp"
#include "taskflow/task_service.hpp"
#include <iostream>
#include <memory>

int main() {
    using namespace taskflow;

    auto repo = std::make_shared<InMemoryTaskRepository>();
    auto strategy = std::make_shared<AssigneePriorityStrategy>();
    TaskService service(repo, strategy);

    EventLog log;
    service.add_listener([&](const std::string& ev, const Task& t) { log.push(ev, t); });

    service.create_task("Design module", "Ramil", 2, "нарисовать схему", "design");
    service.create_task("Implement service", "Ramil", 1, "TaskService", "code");
    service.create_task("Check tests", "QA", 4, "ctest", "qa");

    std::cout << "=== Ramil TaskFlow ===\n";
    for (const auto& t : service.list_by_priority()) {
        std::cout << "#" << t.id << " " << t.title << " [" << t.tag << "] "
                  << t.assignee << " " << to_string(t.status)
                  << " p=" << t.priority << " :: " << t.description << '\n';
    }
    std::cout << "log size=" << log.size() << '\n';
    for (const auto& line : log.lines()) {
        std::cout << "  " << line << '\n';
    }
    return 0;
}
