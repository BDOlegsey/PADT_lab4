#ifndef LAB4_LAZY_SEQUENCE_TPP
#define LAB4_LAZY_SEQUENCE_TPP

#include "lazy_sequence.h"
#include <stdexcept>
#include <string>

namespace lab4 {

// ── constructors ─────────────────────────────────────────────────────────────

template <class T>
LazySequence<T>::LazySequence() : state_(std::make_shared<State>()) {}

template <class T>
LazySequence<T>::LazySequence(const T* items, int count)
    : state_(std::make_shared<State>()) {
    if (count < 0) throw InvalidArgument("LazySequence: count must be non-negative");
    for (int i = 0; i < count; ++i)
        state_->materialized.push_back(items[i]);
    state_->length = Cardinal::Finite((size_t)count);
    state_->generator.SetNextIndex((size_t)count);
}

template <class T>
LazySequence<T>::LazySequence(Rule rule, const T* seed, int seed_count)
    : state_(std::make_shared<State>()) {
    state_->generator = Generator<T>(rule);
    if (seed_count < 0) throw InvalidArgument("LazySequence: seed_count must be non-negative");
    for (int i = 0; i < seed_count; ++i)
        state_->materialized.push_back(seed[i]);
    state_->generator.SetNextIndex((size_t)seed_count);
    state_->length = Cardinal::Omega();
}

template <class T>
LazySequence<T>::LazySequence(const LazySequence<T>& other)
    : state_(other.state_) {}

template <class T>
LazySequence<T>& LazySequence<T>::operator=(const LazySequence<T>& other) {
    state_ = other.state_;
    return *this;
}

// ── private helpers ───────────────────────────────────────────────────────────

template <class T>
void LazySequence<T>::CheckIndex(int index) const {
    if (index < 0)
        throw IndexOutOfRange("index " + std::to_string(index) + " is negative");
    if (state_->length.IsFinite() && (size_t)index >= state_->length.FiniteValue())
        throw IndexOutOfRange("index " + std::to_string(index) + " out of range");
}

template <class T>
void LazySequence<T>::MaterializeNext() {
    if (!state_->generator.HasRule())
        throw EndOfStream("LazySequence: no more elements to materialize");
    T val = state_->generator.GetNext(state_->materialized);
    state_->materialized.push_back(val);
}

template <class T>
void LazySequence<T>::EnsureMaterialized(size_t idx) {
    while (state_->materialized.size() <= idx) {
        MaterializeNext();
    }
}

// ── decomposition ─────────────────────────────────────────────────────────────

template <class T>
T LazySequence<T>::GetFirst() {
    if (state_->length.IsFinite() && state_->length.FiniteValue() == 0)
        throw IndexOutOfRange("GetFirst: sequence is empty");
    EnsureMaterialized(0);
    return state_->materialized[0];
}

template <class T>
T LazySequence<T>::GetLast() {
    if (!state_->length.IsFinite())
        throw InvalidArgument("GetLast: sequence is infinite");
    size_t n = state_->length.FiniteValue();
    if (n == 0) throw IndexOutOfRange("GetLast: sequence is empty");
    EnsureMaterialized(n - 1);
    return state_->materialized[n - 1];
}

template <class T>
T LazySequence<T>::Get(int index) {
    CheckIndex(index);
    EnsureMaterialized((size_t)index);
    return state_->materialized[(size_t)index];
}

template <class T>
LazySequence<T>* LazySequence<T>::GetSubsequence(int start, int end) {
    CheckIndex(start);
    CheckIndex(end);
    if (start > end)
        throw InvalidArgument("GetSubsequence: start > end");
    EnsureMaterialized((size_t)end);
    const T* ptr = state_->materialized.data() + start;
    return new LazySequence<T>(ptr, end - start + 1);
}

template <class T>
Cardinal LazySequence<T>::GetLength() const { return state_->length; }

template <class T>
size_t LazySequence<T>::GetMaterializedCount() const {
    return state_->materialized.size();
}

// ── operations ────────────────────────────────────────────────────────────────

template <class T>
LazySequence<T>* LazySequence<T>::Append(const T& item) {
    if (state_->length.IsFinite()) {
        size_t new_idx = state_->length.FiniteValue();
        // if already materialized up to here, just push
        state_->materialized.push_back(item);
        state_->generator.SetNextIndex(state_->materialized.size());
        state_->length = Cardinal::Finite(new_idx + 1);
    } else {
        // infinite: schedule insert at very large index (logical end - not reachable by rule)
        // We instead use a second tail approach: not practical. 
        // For infinite sequences, Append schedules at the "current pending end":
        // append to tail sequence
        if (!state_->tail) {
            state_->tail = std::make_shared<LazySequence<T>>();
        }
        state_->tail->Append(item);
    }
    return this;
}

template <class T>
LazySequence<T>* LazySequence<T>::Prepend(const T& item) {
    // insert at position 0
    return InsertAt(item, 0);
}

template <class T>
LazySequence<T>* LazySequence<T>::InsertAt(const T& item, int index) {
    if (index < 0)
        throw InvalidArgument("InsertAt: negative index");
    size_t idx = (size_t)index;
    if (state_->length.IsFinite()) {
        size_t len = state_->length.FiniteValue();
        if (idx > len)
            throw IndexOutOfRange("InsertAt: index out of range");
        // materialize up to idx
        EnsureMaterialized(idx > 0 ? idx - 1 : 0);
        if (idx <= state_->materialized.size()) {
            state_->materialized.insert(state_->materialized.begin() + idx, item);
            state_->generator.SetNextIndex(state_->materialized.size());
            state_->length = Cardinal::Finite(len + 1);
        } else {
            // schedule for generator
            state_->generator.ShiftPendingIndices(idx, +1);
            state_->generator.ScheduleInsert(item, idx);
            state_->length = Cardinal::Finite(len + 1);
        }
    } else {
        // infinite: schedule as pending insert
        state_->generator.ShiftPendingIndices(idx, +1);
        state_->generator.ScheduleInsert(item, idx);
    }
    return this;
}

template <class T>
LazySequence<T>* LazySequence<T>::Concat(LazySequence<T>* other) {
    // store other as tail
    state_->tail = std::make_shared<LazySequence<T>>(*other);
    // if this is finite, combined length = this.length + other.length
    if (state_->length.IsFinite() && other->state_->length.IsFinite()) {
        state_->length = state_->length + other->state_->length.FiniteValue();
    } else {
        // if either is infinite, combined length is omega
        state_->length = Cardinal::Omega();
    }
    return this;
}

// ── map / where / reduce ─────────────────────────────────────────────────────

template <class T>
template <class T2>
LazySequence<T2>* LazySequence<T>::Map(std::function<T2(const T&)> f) {
    // materialize what we know, apply f, create finite or rule-based result
    if (state_->length.IsFinite()) {
        size_t len = state_->length.FiniteValue();
        EnsureMaterialized(len > 0 ? len - 1 : 0);
        auto* result = new LazySequence<T2>();
        result->state_->materialized.resize(len);
        for (size_t i = 0; i < len; ++i)
            result->state_->materialized[i] = f(state_->materialized[i]);
        result->state_->length = state_->length;
        result->state_->generator.SetNextIndex(len);
        return result;
    }
    // infinite: capture shared_ptr to this state, build rule
    auto src_state = state_;
    auto* result = new LazySequence<T2>(
        [src_state, f](const std::vector<T2>& mat) -> T2 {
            size_t idx = mat.size();
            // ensure source materialized at idx
            while (src_state->materialized.size() <= idx) {
                T val = src_state->generator.GetNext(src_state->materialized);
                src_state->materialized.push_back(val);
            }
            return f(src_state->materialized[idx]);
        },
        nullptr, 0
    );
    return result;
}

template <class T>
LazySequence<T>* LazySequence<T>::Where(std::function<bool(const T&)> pred) {
    if (state_->length.IsFinite()) {
        size_t len = state_->length.FiniteValue();
        EnsureMaterialized(len > 0 ? len - 1 : 0);
        std::vector<T> filtered;
        for (size_t i = 0; i < len; ++i)
            if (pred(state_->materialized[i]))
                filtered.push_back(state_->materialized[i]);
        return new LazySequence<T>(filtered.data(), (int)filtered.size());
    }
    // infinite: collect on demand -- return finite approximation (not supported for pure infinite)
    // Materialize first 1000 and filter (practical limit)
    const size_t kLimit = 1000;
    std::vector<T> filtered;
    for (size_t i = 0; i < kLimit; ++i) {
        EnsureMaterialized(i);
        if (pred(state_->materialized[i]))
            filtered.push_back(state_->materialized[i]);
    }
    return new LazySequence<T>(filtered.data(), (int)filtered.size());
}

template <class T>
template <class T2>
T2 LazySequence<T>::Reduce(std::function<T2(const T2&, const T&)> f, T2 init, size_t count) {
    T2 acc = init;
    for (size_t i = 0; i < count; ++i) {
        EnsureMaterialized(i);
        acc = f(acc, state_->materialized[i]);
    }
    return acc;
}

template <class T>
LazySequence<T>* LazySequence<T>::Zip(LazySequence<T>* other) {
    // pair-wise: take min(len, other->len) elements
    size_t count;
    if (state_->length.IsFinite() && other->state_->length.IsFinite())
        count = std::min(state_->length.FiniteValue(), other->state_->length.FiniteValue());
    else
        count = 20;  // reasonable default for infinite
    // build pairs as alternating values (interleaved)
    std::vector<T> pairs;
    for (size_t i = 0; i < count; ++i) {
        EnsureMaterialized(i);
        other->EnsureMaterialized(i);
        pairs.push_back(state_->materialized[i]);
        pairs.push_back(other->state_->materialized[i]);
    }
    return new LazySequence<T>(pairs.data(), (int)pairs.size());
}

}  // namespace lab4
#endif
