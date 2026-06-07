#ifndef LAB4_GENERATOR_H
#define LAB4_GENERATOR_H

#include <cstddef>
#include <functional>
#include <vector>
#include "errors.h"
#include "option.h"

namespace lab4 {

template <class T> class LazySequence;

template <class T>
class Generator {
public:
    using Rule = std::function<T(const std::vector<T>&)>;

    explicit Generator(Rule rule);
    Generator();

    Generator(const Generator&) = default;
    Generator& operator=(const Generator&) = default;

    bool HasRule() const;
    bool HasNext() const;

    T GetNext(const std::vector<T>& materialized);
    Option<T> TryGetNext(const std::vector<T>& materialized);

    size_t NextIndex() const;
    void SetNextIndex(size_t idx);

    void ScheduleInsert(T value, size_t idx);
    void ScheduleRemove(size_t idx);
    void ShiftPendingIndices(size_t from_idx, int delta);

    struct PendingOp {
        enum Kind { kInsert, kRemove } kind;
        size_t index;
        T value;
    };

    const std::vector<PendingOp>& Pending() const { return pending_; }

private:
    Rule rule_;
    bool has_rule_;
    size_t next_index_;
    std::vector<PendingOp> pending_;
};

}  // namespace lab4

#include "generator.tpp"
#endif
