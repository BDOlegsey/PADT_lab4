#ifndef LAB4_GENERATOR_H
#define LAB4_GENERATOR_H

#include <cstddef>
#include <functional>
#include <memory>
#include "../lab2/include/dynamic_array.h"
#include "errors.h"
#include "option.h"
#include "ordinal.h"

namespace lab4 {

template <class T> class LazySequence;

template <class T>
class Generator {
public:
    using Rule = std::function<T(const lab2::DynamicArray<T>&)>;
    using Ptr = std::shared_ptr<Generator<T>>;

    explicit Generator(Rule rule);
    Generator();

    static Ptr FromArray(const T* items, int count);
    static Ptr FromRule(Rule rule, const T* seed, int seed_count);

    Generator(const Generator&) = default;
    Generator& operator=(const Generator&) = default;

    bool HasRule() const;
    bool HasNext() const;

    int MaterializedCount() const;
    int PrefixLength() const;
    const lab2::DynamicArray<T>& Materialized() const;
    Ptr Suffix() const;

    size_t NextIndex() const;
    void SetNextIndex(size_t idx);

    void ScheduleInsert(const T& value, int index);
    void ScheduleRemove(int index);
    void ShiftPendingIndices(int from_index, int delta);

    T GetFinite(int index);
    T GetAt(const Ordinal& index);
    void Append(const T& item, bool infinite);
    void InsertAt(const T& item, int index, bool infinite);
    void Concat(Ptr other, bool self_infinite, bool other_infinite);
    static void LinkSuffix(Ptr head, Ptr other);

    struct PendingOp {
        enum Kind { kInsert, kRemove } kind;
        size_t index;
        T value;
    };

    const lab2::DynamicArray<PendingOp>& Pending() const { return pending_; }

    static Ptr TailOf(Ptr head);
    bool TryParseOmegaPlus(const Ordinal& index, uint64_t* tail);

private:
    T ProduceNext();
    void MaterializeNext();
    void EnsureMaterialized(int index);

    Rule rule_;
    bool has_rule_;
    size_t next_index_;
    lab2::DynamicArray<T> materialized_;
    int prefix_length_;
    Ptr suffix_;
    lab2::DynamicArray<T> post_omega_;
    lab2::DynamicArray<PendingOp> pending_;
};

}  // namespace lab4

#include "generator.tpp"
#endif
