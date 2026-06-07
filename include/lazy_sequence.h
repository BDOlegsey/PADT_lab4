#ifndef LAB4_LAZY_SEQUENCE_H
#define LAB4_LAZY_SEQUENCE_H

#include <cstddef>
#include <functional>
#include <memory>
#include <vector>
#include "cardinal.h"
#include "errors.h"
#include "generator.h"
#include "option.h"

namespace lab4 {

template <class T>
class LazySequence {
public:
    using Rule = std::function<T(const std::vector<T>&)>;

    LazySequence();
    LazySequence(const T* items, int count);
    // from rule with seed: rule uses already-materialized prefix as history
    LazySequence(Rule rule, const T* seed, int seed_count);
    // copy (shares memoized state)
    LazySequence(const LazySequence<T>& other);
    LazySequence<T>& operator=(const LazySequence<T>& other);
    ~LazySequence() = default;

    // --- decomposition ---
    T GetFirst();
    T GetLast();   // may not terminate for infinite sequences
    T Get(int index);
    LazySequence<T>* GetSubsequence(int start, int end);

    Cardinal GetLength() const;
    size_t GetMaterializedCount() const;

    // --- operations (mutate this, return this) ---
    LazySequence<T>* Append(const T& item);
    LazySequence<T>* Prepend(const T& item);
    LazySequence<T>* InsertAt(const T& item, int index);

    // concat: second sequence appended logically after this
    LazySequence<T>* Concat(LazySequence<T>* other);

    // --- map/where/reduce ---
    template <class T2>
    LazySequence<T2>* Map(std::function<T2(const T&)> f);

    LazySequence<T>* Where(std::function<bool(const T&)> pred);

    template <class T2>
    T2 Reduce(std::function<T2(const T2&, const T&)> f, T2 init, size_t count);

    LazySequence<T>* Zip(LazySequence<T>* other);  // pairs stored as T (requires T == pair)

private:
    struct State {
        std::vector<T> materialized;
        Generator<T> generator;
        Cardinal length;
        // second sequence for Concat (nullptr if none)
        std::shared_ptr<LazySequence<T>> tail;

        State() : generator(), length(Cardinal::Finite(0)) {}
    };

    std::shared_ptr<State> state_;

    void EnsureMaterialized(size_t idx);
    void MaterializeNext();

    void CheckIndex(int index) const;
};

}  // namespace lab4

#include "lazy_sequence.tpp"
#endif
