#pragma once
#include "taskflow/task.hpp"
#include <memory>

namespace taskflow {

class IPriorityStrategy {
public:
    virtual ~IPriorityStrategy() = default;
    virtual int score(const Task& task) const = 0;
};

class DefaultPriorityStrategy final : public IPriorityStrategy {
public:
    int score(const Task& task) const override;
};

class ClosedLastPriorityStrategy final : public IPriorityStrategy {
public:
    int score(const Task& task) const override;
};

// ЛР2: без исполнителя — в конец, иначе по priority
class AssigneePriorityStrategy final : public IPriorityStrategy {
public:
    int score(const Task& task) const override;
};

using PriorityStrategyPtr = std::shared_ptr<IPriorityStrategy>;

} // namespace taskflow
