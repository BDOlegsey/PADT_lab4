#ifndef LAB4_LAZY_SEQUENCE_H
#define LAB4_LAZY_SEQUENCE_H

#include <cstddef>
#include <functional>
#include <memory>

#include "../lab2/include/dynamic_array.h"
#include "cardinal.h"
#include "errors.h"
#include "generator.h"
#include "ordinal.h"

namespace lab4 {

template <class T>
class LazySequence {
public:
    using Rule = std::function<T(const lab2::DynamicArray<T>&)>;
    using GeneratorPtr = typename Generator<T>::Ptr;

    LazySequence();
    LazySequence(const T* items, int count);
    LazySequence(Rule rule, const T* seed, int seed_count);

    LazySequence(const LazySequence<T>& other);
    LazySequence<T>& operator=(const LazySequence<T>& other);
    ~LazySequence() = default;

    T GetFirst();
    T GetLast();
    T Get(int index);
    T Get(const Ordinal& index);
    LazySequence<T>* GetSubsequence(int start, int end);

    Cardinal GetLength() const;
    size_t GetMaterializedCount() const;

    LazySequence<T>* Append(const T& item);
    LazySequence<T>* Prepend(const T& item);
    LazySequence<T>* InsertAt(const T& item, int index);
    LazySequence<T>* Concat(LazySequence<T>* other);

    template <class T2>
    LazySequence<T2>* Map(std::function<T2(const T&)> f);

    LazySequence<T>* Where(std::function<bool(const T&)> pred);

    template <class T2>
    T2 Reduce(std::function<T2(const T2&, const T&)> f, T2 init, size_t count);

    LazySequence<T>* Zip(LazySequence<T>* other);

private:
    GeneratorPtr gen_;
    Cardinal length_;

    LazySequence(GeneratorPtr gen, Cardinal length);

    void CheckIndex(int index) const;
    bool IsInfinite() const;
};

}  // namespace lab4

#include "lazy_sequence.tpp"

#endif
