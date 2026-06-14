#ifndef LAB4_GENERATOR_TPP
#define LAB4_GENERATOR_TPP

#include "generator.h"
#include <stdexcept>
#include <string>

namespace lab4 {

namespace {

template <class T>
void ArrayPushBack(lab2::DynamicArray<T>& array, const T& value) {
    int size = array.GetSize();
    array.Resize(size + 1);
    array.Set(size, value);
}

template <class T>
void ArrayInsertAt(lab2::DynamicArray<T>& array, int index, const T& value) {
    int size = array.GetSize();
    if (index < 0 || index > size) {
        throw IndexOutOfRange("ArrayInsertAt: index out of range");
    }
    array.Resize(size + 1);
    for (int i = size; i > index; --i) {
        array.Set(i, array.Get(i - 1));
    }
    array.Set(index, value);
}

template <class T>
void ArrayRemoveAt(lab2::DynamicArray<T>& array, int index) {
    int size = array.GetSize();
    if (index < 0 || index >= size) {
        throw IndexOutOfRange("ArrayRemoveAt: index out of range");
    }
    for (int i = index + 1; i < size; ++i) {
        array.Set(i - 1, array.Get(i));
    }
    array.Resize(size - 1);
}

}  // namespace

template <class T>
Generator<T>::Generator(Rule rule)
    : rule_(rule), has_rule_(true), next_index_(0), prefix_length_(0), suffix_(nullptr) {}

template <class T>
Generator<T>::Generator()
    : has_rule_(false), next_index_(0), prefix_length_(0), suffix_(nullptr) {}

template <class T>
typename Generator<T>::Ptr Generator<T>::FromArray(const T* items, int count) {
    Ptr gen(new Generator<T>());
    if (count < 0) {
        throw InvalidArgument("Generator: count must be non-negative");
    }
    if (count > 0) {
        gen->materialized_ = lab2::DynamicArray<T>(items, count);
    }
    gen->prefix_length_ = count;
    gen->next_index_ = static_cast<size_t>(count);
    return gen;
}

template <class T>
typename Generator<T>::Ptr Generator<T>::FromRule(Rule rule, const T* seed, int seed_count) {
    if (seed_count < 0) {
        throw InvalidArgument("Generator: seed_count must be non-negative");
    }
    Ptr gen(new Generator<T>(rule));
    if (seed_count > 0) {
        if (!seed) {
            throw InvalidArgument("Generator: seed pointer is null");
        }
        gen->materialized_ = lab2::DynamicArray<T>(seed, seed_count);
    }
    gen->prefix_length_ = seed_count;
    gen->next_index_ = static_cast<size_t>(seed_count);
    return gen;
}

template <class T>
bool Generator<T>::HasRule() const {
    return has_rule_;
}

template <class T>
bool Generator<T>::HasNext() const {
    return has_rule_ || suffix_ != nullptr || materialized_.GetSize() > 0;
}

template <class T>
int Generator<T>::MaterializedCount() const {
    return materialized_.GetSize();
}

template <class T>
int Generator<T>::PrefixLength() const {
    return prefix_length_;
}

template <class T>
const lab2::DynamicArray<T>& Generator<T>::Materialized() const {
    return materialized_;
}

template <class T>
typename Generator<T>::Ptr Generator<T>::Suffix() const {
    return suffix_;
}

template <class T>
size_t Generator<T>::NextIndex() const {
    return next_index_;
}

template <class T>
void Generator<T>::SetNextIndex(size_t idx) {
    next_index_ = idx;
}

template <class T>
void Generator<T>::ScheduleInsert(const T& value, int index) {
    if (index < 0) {
        throw IndexOutOfRange("Generator: insert index is negative");
    }
    PendingOp op;
    op.kind = PendingOp::kInsert;
    op.index = static_cast<size_t>(index);
    op.value = value;
    ArrayPushBack(pending_, op);
}

template <class T>
void Generator<T>::ScheduleRemove(int index) {
    if (index < 0) {
        throw IndexOutOfRange("Generator: remove index is negative");
    }
    PendingOp op;
    op.kind = PendingOp::kRemove;
    op.index = static_cast<size_t>(index);
    ArrayPushBack(pending_, op);
}

template <class T>
void Generator<T>::ShiftPendingIndices(int from_index, int delta) {
    if (from_index < 0) {
        throw IndexOutOfRange("Generator: from_index is negative");
    }
    for (int i = 0; i < pending_.GetSize(); ++i) {
        PendingOp& op = pending_.Get(i);
        if (op.index >= static_cast<size_t>(from_index)) {
            int new_index = static_cast<int>(op.index) + delta;
            if (new_index < 0) {
                op.index = 0;
            } else {
                op.index = static_cast<size_t>(new_index);
            }
        }
    }
}

template <class T>
bool Generator<T>::TryParseOmegaPlus(const Ordinal& index, uint64_t* tail) {
    const lab2::DynamicArray<Ordinal::Term>& terms = index.Terms();
    int size = terms.GetSize();
    if (size == 0 || size > 2) {
        return false;
    }
    if (!(*terms.Get(0).exponent == Ordinal::Finite(1)) || terms.Get(0).coef != 1) {
        return false;
    }
    if (size == 1) {
        *tail = 0;
        return true;
    }
    if (!terms.Get(1).exponent->IsZero()) {
        return false;
    }
    *tail = terms.Get(1).coef;
    return true;
}

template <class T>
typename Generator<T>::Ptr Generator<T>::TailOf(Ptr head) {
    if (!head) {
        return head;
    }
    while (head->suffix_) {
        head = head->suffix_;
    }
    return head;
}

template <class T>
void Generator<T>::LinkSuffix(Ptr head, Ptr other) {
    if (!head) {
        throw InvalidArgument("Generator: head generator is null");
    }
    Ptr tail = TailOf(head);
    tail->suffix_ = other;
}

template <class T>
T Generator<T>::ProduceNext() {
    for (int i = 0; i < pending_.GetSize(); ++i) {
        PendingOp op = pending_.Get(i);
        if (op.kind == PendingOp::kInsert && op.index == next_index_) {
            ArrayRemoveAt(pending_, i);
            ++next_index_;
            return op.value;
        }
    }

    for (int i = 0; i < pending_.GetSize(); ++i) {
        PendingOp op = pending_.Get(i);
        if (op.kind == PendingOp::kRemove && op.index == next_index_) {
            ArrayRemoveAt(pending_, i);
            if (!has_rule_) {
                throw EndOfStream("Generator: remove past end of sequence");
            }
            rule_(materialized_);
            ++next_index_;
            return ProduceNext();
        }
    }

    if (!has_rule_) {
        throw EndOfStream("Generator: no more elements");
    }

    T value = rule_(materialized_);
    ++next_index_;
    return value;
}

template <class T>
void Generator<T>::MaterializeNext() {
    if (has_rule_) {
        ArrayPushBack(materialized_, ProduceNext());
        return;
    }

    if (suffix_) {
        int offset = materialized_.GetSize() - prefix_length_;
        ArrayPushBack(materialized_, suffix_->GetFinite(offset));
        return;
    }

    throw EndOfStream("Generator: no more elements to materialize");
}

template <class T>
void Generator<T>::EnsureMaterialized(int index) {
    if (index < 0) {
        throw IndexOutOfRange("Generator: index is negative");
    }
    while (materialized_.GetSize() <= index) {
        if (!has_rule_ && !suffix_) {
            throw IndexOutOfRange("Generator: index out of finite range");
        }
        MaterializeNext();
    }
}

template <class T>
T Generator<T>::GetFinite(int index) {
    if (index < 0) {
        throw IndexOutOfRange("Generator: index is negative");
    }

    if (!has_rule_ && suffix_ && index >= prefix_length_) {
        return suffix_->GetFinite(index - prefix_length_);
    }

    EnsureMaterialized(index);
    return materialized_.Get(index);
}

template <class T>
T Generator<T>::GetAt(const Ordinal& index) {
    if (index.IsFinite()) {
        return GetFinite(static_cast<int>(index.FiniteValue()));
    }

    uint64_t tail = 0;
    if (!TryParseOmegaPlus(index, &tail)) {
        throw InvalidArgument("Generator: unsupported ordinal index");
    }

    if (tail < static_cast<uint64_t>(post_omega_.GetSize())) {
        return post_omega_.Get(static_cast<int>(tail));
    }
    if (suffix_) {
        int suffix_index = static_cast<int>(tail - post_omega_.GetSize());
        return suffix_->GetFinite(suffix_index);
    }

    throw IndexOutOfRange("Generator: ordinal index beyond sequence end");
}

template <class T>
void Generator<T>::Append(const T& item, bool infinite) {
    if (infinite) {
        ArrayPushBack(post_omega_, item);
        return;
    }
    ArrayPushBack(materialized_, item);
    ++prefix_length_;
    next_index_ = static_cast<size_t>(materialized_.GetSize());
}

template <class T>
void Generator<T>::InsertAt(const T& item, int index, bool infinite) {
    if (index < 0) {
        throw IndexOutOfRange("Generator: insert index is negative");
    }

    if (!infinite) {
        if (index > prefix_length_) {
            throw IndexOutOfRange("Generator: insert index out of range");
        }

        if (index <= materialized_.GetSize()) {
            if (index > 0) {
                EnsureMaterialized(index - 1);
            }
            ArrayInsertAt(materialized_, index, item);
            ++prefix_length_;
            next_index_ = static_cast<size_t>(materialized_.GetSize());
            return;
        }

        ShiftPendingIndices(index, +1);
        ScheduleInsert(item, index);
        ++prefix_length_;
        return;
    }

    ShiftPendingIndices(index, +1);
    ScheduleInsert(item, index);
}

template <class T>
void Generator<T>::Concat(Ptr other, bool self_infinite, bool other_infinite) {
    if (!other) {
        throw InvalidArgument("Generator: other is null");
    }

    if (!self_infinite && !other_infinite) {
        int count = other->materialized_.GetSize();
        for (int i = 0; i < count; ++i) {
            ArrayPushBack(materialized_, other->materialized_.Get(i));
        }
        prefix_length_ = materialized_.GetSize();
        next_index_ = static_cast<size_t>(prefix_length_);
        return;
    }

    if (!self_infinite && other_infinite) {
        suffix_ = other;
        return;
    }

    if (self_infinite && !other_infinite) {
        int count = other->materialized_.GetSize();
        for (int i = 0; i < count; ++i) {
            ArrayPushBack(post_omega_, other->materialized_.Get(i));
        }
        return;
    }

    suffix_ = other;
}

}  // namespace lab4

#endif