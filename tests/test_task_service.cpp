#include "simple_test.hpp"
#include "taskflow/event_log.hpp"
#include "taskflow/in_memory_task_repository.hpp"
#include "taskflow/priority_strategy.hpp"
#include "taskflow/task_service.hpp"
#include <memory>
#include <string>

using namespace taskflow;

static TaskService svc() {
    return TaskService(std::make_shared<InMemoryTaskRepository>(),
                       std::make_shared<DefaultPriorityStrategy>());
}

TEST_CASE("create ok") {
    auto s = svc();
    auto t = s.create_task("T", "Ramil", 2, "d", "tag1");
    CHECK_TRUE(t.id > 0);
    CHECK_EQ(t.tag, std::string("tag1"));
}

TEST_CASE("priority bad") {
    auto s = svc();
    CHECK_THROWS(s.create_task("T", "R", 8));
}

TEST_CASE("status flow") {
    auto s = svc();
    auto t = s.create_task("T", "R", 1);
    CHECK_TRUE(s.transition(t.id, TaskStatus::InProgress));
    CHECK_TRUE(s.transition(t.id, TaskStatus::Done));
    CHECK_TRUE(!s.transition(t.id, TaskStatus::InProgress));
}

TEST_CASE("assign ok") {
    auto s = svc();
    auto t = s.create_task("T", "A", 2);
    CHECK_TRUE(s.assign(t.id, "B"));
    CHECK_EQ(s.find(t.id)->assignee, std::string("B"));
}

TEST_CASE("assign denied for Done") {
    auto s = svc();
    auto t = s.create_task("T", "A", 2);
    s.transition(t.id, TaskStatus::InProgress);
    s.transition(t.id, TaskStatus::Done);
    CHECK_TRUE(!s.assign(t.id, "B"));
}

TEST_CASE("long description fails") {
    auto s = svc();
    CHECK_THROWS(s.create_task("T", "A", 2, std::string(1001, 'q')));
}

TEST_CASE("description 1000 ok") {
    auto s = svc();
    auto t = s.create_task("T", "A", 2, std::string(1000, 'q'));
    CHECK_EQ(t.description.size(), static_cast<std::size_t>(1000));
}

TEST_CASE("empty title") { CHECK_THROWS(svc().create_task("", "A", 2)); }
TEST_CASE("empty assignee") { CHECK_THROWS(svc().create_task("T", "", 2)); }
TEST_CASE("priority low") { CHECK_THROWS(svc().create_task("T", "A", 0)); }
TEST_CASE("priority high") { CHECK_THROWS(svc().create_task("T", "A", 6)); }

TEST_CASE("cancel ok") {
    auto s = svc();
    auto t = s.create_task("T", "A", 3);
    CHECK_TRUE(s.transition(t.id, TaskStatus::Cancelled));
}

TEST_CASE("assign empty throws") {
    auto s = svc();
    auto t = s.create_task("T", "A", 3);
    CHECK_THROWS(s.assign(t.id, ""));
}

TEST_CASE("missing id") { CHECK_TRUE(!svc().find(42).has_value()); }

TEST_CASE("repo integration") {
    auto repo = std::make_shared<InMemoryTaskRepository>();
    Task d; d.title = "x"; d.assignee = "y"; d.priority = 2;
    auto c = repo->create(d);
    CHECK_TRUE(repo->find_by_id(c.id).has_value());
    c.status = TaskStatus::InProgress;
    CHECK_TRUE(repo->update(c));
    CHECK_TRUE(repo->list().size() == 1);
}

TEST_CASE("order default") {
    auto s = svc();
    auto low = s.create_task("L", "a", 5);
    auto high = s.create_task("H", "b", 1);
    auto o = s.list_by_priority();
    CHECK_EQ(o.front().id, low.id);
    CHECK_EQ(o.back().id, high.id);
}
