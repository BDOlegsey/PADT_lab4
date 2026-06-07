#ifndef LAB4_GENERATOR_TPP
#define LAB4_GENERATOR_TPP

#include "generator.h"
#include <stdexcept>

namespace lab4 {

template <class T>
Generator<T>::Generator(Rule rule)
    : rule_(rule), has_rule_(true), next_index_(0) {}

template <class T>
Generator<T>::Generator()
    : has_rule_(false), next_index_(0) {}

template <class T>
bool Generator<T>::HasRule() const { return has_rule_; }

template <class T>
bool Generator<T>::HasNext() const { return has_rule_; }

template <class T>
size_t Generator<T>::NextIndex() const { return next_index_; }

template <class T>
void Generator<T>::SetNextIndex(size_t idx) { next_index_ = idx; }

template <class T>
void Generator<T>::ScheduleInsert(T value, size_t idx) {
    PendingOp op;
    op.kind  = PendingOp::kInsert;
    op.index = idx;
    op.value = value;
    pending_.push_back(op);
}

template <class T>
void Generator<T>::ScheduleRemove(size_t idx) {
    PendingOp op;
    op.kind  = PendingOp::kRemove;
    op.index = idx;
    pending_.push_back(op);
}

template <class T>
void Generator<T>::ShiftPendingIndices(size_t from_idx, int delta) {
    for (auto& op : pending_) {
        if (op.index >= from_idx) {
            if (delta < 0 && op.index < (size_t)(-delta)) op.index = 0;
            else op.index = (size_t)((int)op.index + delta);
        }
    }
}

template <class T>
T Generator<T>::GetNext(const std::vector<T>& materialized) {
    
    for (size_t i = 0; i < pending_.size(); ++i) {
        auto& op = pending_[i];
        if (op.kind == PendingOp::kInsert && op.index == next_index_) {
            T val = op.value;
            pending_.erase(pending_.begin() + i);
            ++next_index_;
            return val;
        }
    }
    for (size_t i = 0; i < pending_.size(); ++i) {
        auto& op = pending_[i];
        if (op.kind == PendingOp::kRemove && op.index == next_index_) {
            pending_.erase(pending_.begin() + i);
            if (!has_rule_) throw EndOfStream("generator: remove past end of sequence");
            rule_(materialized); 
            ++next_index_;
            return GetNext(materialized);
        }
    }
    if (!has_rule_) throw EndOfStream("generator: no more elements");
    T val = rule_(materialized);
    ++next_index_;
    return val;
}

template <class T>
Option<T> Generator<T>::TryGetNext(const std::vector<T>& materialized) {
    try {
        return Option<T>::Some(GetNext(materialized));
    } catch (...) {
        return Option<T>::None();
    }
}

}  // namespace lab4
#endif
