#ifndef LAB4_GENERATOR_H
#define LAB4_GENERATOR_H

#include <cstddef>
#include <functional>
#include <vector>
#include "errors.h"
#include "option.h"

namespace lab4 {

template <class T> class LazySequence;

// internal engine of LazySequence: holds the rule and pending insert/remove ops
template <class T>
class Generator {
public:
    using Rule = std::function<T(const std::vector<T>&)>;

    // rule-based generator: rule uses already-materialized elements
    explicit Generator(Rule rule);
    // no-rule generator (finite sequence, no further generation)
    Generator();

    Generator(const Generator&) = default;
    Generator& operator=(const Generator&) = default;

    bool HasRule() const;
    bool HasNext() const;  // always true if rule, false if no rule

    // generate next element using rule; advances next_index_
    T GetNext(const std::vector<T>& materialized);
    Option<T> TryGetNext(const std::vector<T>& materialized);

    size_t NextIndex() const;
    void SetNextIndex(size_t idx);

    // schedule insert of value before logical index idx
    void ScheduleInsert(T value, size_t idx);
    // schedule remove at logical index idx
    void ScheduleRemove(size_t idx);
    // shift pending ops with index >= from_idx by delta
    void ShiftPendingIndices(size_t from_idx, int delta);

    struct PendingOp {
        enum Kind { kInsert, kRemove } kind;
        size_t index;
        T value;  // used for kInsert
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
