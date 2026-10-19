#include "simple_test.hpp"
#include "taskflow/event_log.hpp"
#include "taskflow/in_memory_task_repository.hpp"
#include "taskflow/priority_strategy.hpp"
#include "taskflow/repository_factory.hpp"
#include "taskflow/sqlite_task_repository.hpp"
#include "taskflow/task_service.hpp"
#include <memory>
#include <string>

using namespace taskflow;

TEST_CASE("factory memory") {
    Config cfg; cfg.backend = "memory";
    auto repo = make_repository(cfg);
    Task d; d.title = "a"; d.assignee = "b"; d.priority = 3;
    auto t = repo->create(d);
    CHECK_TRUE(repo->find_by_id(t.id).has_value());
}

TEST_CASE("closed last") {
    auto repo = std::make_shared<InMemoryTaskRepository>();
    TaskService s(repo, std::make_shared<ClosedLastPriorityStrategy>());
    auto a = s.create_task("A", "u", 1);
    auto b = s.create_task("B", "u", 5);
    s.transition(a.id, TaskStatus::InProgress);
    s.transition(a.id, TaskStatus::Done);
    CHECK_EQ(s.list_by_priority().front().id, b.id);
}

TEST_CASE("factory sqlite") {
    Config cfg; cfg.backend = "sqlite"; cfg.db_file = "ramil_tmp.db";
    auto repo = make_repository(cfg);
    auto* p = dynamic_cast<SQLiteTaskRepository*>(repo.get());
    CHECK_TRUE(p != nullptr);
    CHECK_EQ(p->file(), std::string("ramil_tmp.db"));
}

TEST_CASE("factory bad backend") {
    Config cfg; cfg.backend = "mongo";
    CHECK_THROWS(make_repository(cfg));
}

TEST_CASE("assignee strategy empty last") {
    auto repo = std::make_shared<InMemoryTaskRepository>();
    TaskService s(repo, std::make_shared<AssigneePriorityStrategy>());
    auto with_user = s.create_task("W", "Ramil", 3);
    Task empty; empty.title = "E"; empty.assignee = "tmp"; empty.priority = 1;
    empty = repo->create(empty);
    empty.assignee = "";
    repo->update(empty);
    auto o = s.list_by_priority();
    CHECK_EQ(o.front().id, with_user.id);
    CHECK_EQ(o.back().id, empty.id);
}

TEST_CASE("observer event log") {
    TaskService s(std::make_shared<InMemoryTaskRepository>(),
                  std::make_shared<DefaultPriorityStrategy>());
    EventLog log;
    s.add_listener([&](const std::string& ev, const Task& t) { log.push(ev, t); });
    auto t = s.create_task("Obs", "Ramil", 2);
    CHECK_TRUE(log.size() == 1);
    CHECK_TRUE(log.lines().front().find("TaskCreated") != std::string::npos);
    CHECK_TRUE(log.lines().front().find(std::to_string(t.id)) != std::string::npos);
}

TEST_CASE("sqlite stub throws") {
    SQLiteTaskRepository r("z.db");
    CHECK_THROWS(r.list());
}
