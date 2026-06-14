#ifndef LAB4_LAZY_SEQUENCE_TPP
#define LAB4_LAZY_SEQUENCE_TPP

#include "lazy_sequence.h"

#include <algorithm>
#include <string>

namespace lab4 {

namespace {

template <class T>
void LazySequenceArrayPushBack(lab2::DynamicArray<T>& array, const T& value) {
    int size = array.GetSize();
    array.Resize(size + 1);
    array.Set(size, value);
}

template <class T>
LazySequence<T>* FromDynamicArray(const lab2::DynamicArray<T>& array) {
    int count = array.GetSize();
    if (count == 0) {
        return new LazySequence<T>();
    }

    T* buffer = new T[count];
    for (int i = 0; i < count; ++i) {
        buffer[i] = array.Get(i);
    }

    LazySequence<T>* sequence = new LazySequence<T>(buffer, count);
    delete[] buffer;
    return sequence;
}

}  // namespace

template <class T>
LazySequence<T>::LazySequence()
    : gen_(Generator<T>::FromArray(nullptr, 0)),
      length_(Cardinal::Finite(0)) {}

template <class T>
LazySequence<T>::LazySequence(const T* items, int count)
    : gen_(Generator<T>::FromArray(items, count)),
      length_(Cardinal::Finite(static_cast<size_t>(count))) {}

template <class T>
LazySequence<T>::LazySequence(Rule rule, const T* seed, int seed_count)
    : gen_(Generator<T>::FromRule(rule, seed, seed_count)),
      length_(Cardinal::Omega()) {}

template <class T>
LazySequence<T>::LazySequence(GeneratorPtr gen, Cardinal length)
    : gen_(gen),
      length_(length) {}

template <class T>
LazySequence<T>::LazySequence(const LazySequence<T>& other)
    : gen_(other.gen_),
      length_(other.length_) {}

template <class T>
LazySequence<T>& LazySequence<T>::operator=(const LazySequence<T>& other) {
    gen_ = other.gen_;
    length_ = other.length_;
    return *this;
}

template <class T>
bool LazySequence<T>::IsInfinite() const {
    return length_.IsOmega();
}

template <class T>
void LazySequence<T>::CheckIndex(int index) const {
    if (index < 0) {
        throw IndexOutOfRange("index " + std::to_string(index) + " is negative");
    }
    if (length_.IsFinite() &&
        static_cast<size_t>(index) >= length_.FiniteValue()) {
        throw IndexOutOfRange("index " + std::to_string(index) + " out of range");
    }
}

template <class T>
T LazySequence<T>::GetFirst() {
    if (length_.IsFinite() && length_.FiniteValue() == 0) {
        throw IndexOutOfRange("GetFirst: sequence is empty");
    }
    return gen_->GetFinite(0);
}

template <class T>
T LazySequence<T>::GetLast() {
    if (!length_.IsFinite()) {
        throw InvalidArgument("GetLast: sequence is infinite");
    }
    size_t last = length_.FiniteValue();
    if (last == 0) {
        throw IndexOutOfRange("GetLast: sequence is empty");
    }
    return gen_->GetFinite(static_cast<int>(last - 1));
}

template <class T>
T LazySequence<T>::Get(int index) {
    CheckIndex(index);
    return gen_->GetFinite(index);
}

template <class T>
T LazySequence<T>::Get(const Ordinal& index) {
    if (index.IsFinite() && length_.IsFinite() &&
        index.FiniteValue() >= length_.FiniteValue()) {
        throw IndexOutOfRange("ordinal index out of finite range");
    }
    return gen_->GetAt(index);
}

template <class T>
LazySequence<T>* LazySequence<T>::GetSubsequence(int start, int end) {
    CheckIndex(start);
    CheckIndex(end);
    if (start > end) {
        throw InvalidArgument("GetSubsequence: start > end");
    }

    int count = end - start + 1;
    lab2::DynamicArray<T> items(count);
    for (int i = 0; i < count; ++i) {
        items.Set(i, gen_->GetFinite(start + i));
    }
    return FromDynamicArray(items);
}

template <class T>
Cardinal LazySequence<T>::GetLength() const {
    return length_;
}

template <class T>
size_t LazySequence<T>::GetMaterializedCount() const {
    return static_cast<size_t>(gen_->MaterializedCount());
}

template <class T>
LazySequence<T>* LazySequence<T>::Append(const T& item) {
    gen_->Append(item, IsInfinite());
    if (length_.IsFinite()) {
        length_ = Cardinal::Finite(length_.FiniteValue() + 1);
    }
    return this;
}

template <class T>
LazySequence<T>* LazySequence<T>::Prepend(const T& item) {
    return InsertAt(item, 0);
}

template <class T>
LazySequence<T>* LazySequence<T>::InsertAt(const T& item, int index) {
    gen_->InsertAt(item, index, IsInfinite());
    if (length_.IsFinite()) {
        length_ = Cardinal::Finite(length_.FiniteValue() + 1);
    }
    return this;
}

template <class T>
LazySequence<T>* LazySequence<T>::Concat(LazySequence<T>* other) {
    if (!other) {
        throw InvalidArgument("Concat: other sequence is null");
    }

    bool self_inf = IsInfinite();
    bool other_inf = other->IsInfinite();

    if (self_inf && other_inf) {
        Generator<T>::LinkSuffix(gen_, other->gen_);
    } else {
        gen_->Concat(other->gen_, self_inf, other_inf);
    }

    if (length_.IsFinite() && other->length_.IsFinite()) {
        length_ = length_ + other->length_.FiniteValue();
    } else {
        length_ = Cardinal::Omega();
    }
    return this;
}

template <class T>
template <class T2>
LazySequence<T2>* LazySequence<T>::Map(std::function<T2(const T&)> f) {
    if (length_.IsFinite()) {
        size_t len = length_.FiniteValue();
        if (len > 0) {
            gen_->GetFinite(static_cast<int>(len - 1));
        }

        lab2::DynamicArray<T2> mapped(static_cast<int>(len));
        for (size_t i = 0; i < len; ++i) {
            mapped.Set(static_cast<int>(i), f(gen_->GetFinite(static_cast<int>(i))));
        }
        return FromDynamicArray(mapped);
    }

    struct MapContext {
        GeneratorPtr source;
        std::function<T2(const T&)> fn;
    };

    auto ctx = std::make_shared<MapContext>();
    ctx->source = gen_;
    ctx->fn = f;

    auto rule = [ctx](const lab2::DynamicArray<T2>& materialized) -> T2 {
        int index = materialized.GetSize();
        T value = ctx->source->GetFinite(index);
        return ctx->fn(value);
    };

    return new LazySequence<T2>(rule, nullptr, 0);
}

template <class T>
LazySequence<T>* LazySequence<T>::Where(std::function<bool(const T&)> pred) {
    if (length_.IsFinite()) {
        size_t len = length_.FiniteValue();
        lab2::DynamicArray<T> filtered;
        for (size_t i = 0; i < len; ++i) {
            T value = gen_->GetFinite(static_cast<int>(i));
            if (pred(value)) {
                LazySequenceArrayPushBack(filtered, value);
            }
        }
        return FromDynamicArray(filtered);
    }

    const int kLimit = 1000;
    lab2::DynamicArray<T> filtered;
    for (int i = 0; i < kLimit; ++i) {
        T value = gen_->GetFinite(i);
        if (pred(value)) {
            LazySequenceArrayPushBack(filtered, value);
        }
    }
    return FromDynamicArray(filtered);
}

template <class T>
template <class T2>
T2 LazySequence<T>::Reduce(std::function<T2(const T2&, const T&)> f, T2 init, size_t count) {
    T2 acc = init;
    for (size_t i = 0; i < count; ++i) {
        acc = f(acc, gen_->GetFinite(static_cast<int>(i)));
    }
    return acc;
}

template <class T>
LazySequence<T>* LazySequence<T>::Zip(LazySequence<T>* other) {
    if (!other) {
        throw InvalidArgument("Zip: other sequence is null");
    }

    int count = 0;
    if (length_.IsFinite() && other->length_.IsFinite()) {
        count = static_cast<int>(
            std::min(length_.FiniteValue(), other->length_.FiniteValue()));
    } else {
        count = 20;
    }

    lab2::DynamicArray<T> pairs(count * 2);
    for (int i = 0; i < count; ++i) {
        pairs.Set(i * 2, gen_->GetFinite(i));
        pairs.Set(i * 2 + 1, other->gen_->GetFinite(i));
    }
    return FromDynamicArray(pairs);
}

}  // namespace lab4

#endif
