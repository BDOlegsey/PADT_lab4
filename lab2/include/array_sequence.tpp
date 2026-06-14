#ifndef LAB2_ARRAY_SEQUENCE_TPP
#define LAB2_ARRAY_SEQUENCE_TPP

#include <string>

namespace lab2 {

template <class T>
class ArraySequence<T>::ArrayEnumerator : public IEnumerator<T> {
public:
    explicit ArrayEnumerator(const DynamicArray<T>* data)
        : data_(data), position_(-1) {}

    bool MoveNext() override {
        if (position_ + 1 >= data_->GetSize()) return false;
        ++position_;
        return true;
    }

    T Current() const override {
        if (position_ < 0) throw InvalidArgument("Enumerator: MoveNext not called");
        return data_->Get(position_);
    }

    void Reset() override { position_ = -1; }

private:
    const DynamicArray<T>* data_;
    int position_;
};

template <class T>
ArraySequence<T>::ArraySequence() : items_() {}

template <class T>
ArraySequence<T>::ArraySequence(const T* items, int count) : items_(items, count) {}

template <class T>
ArraySequence<T>::ArraySequence(const DynamicArray<T>& source) : items_(source) {}

template <class T>
ArraySequence<T>::ArraySequence(const ArraySequence<T>& other) : items_(other.items_) {}

template <class T>
ArraySequence<T>& ArraySequence<T>::operator=(const ArraySequence<T>& other) {
    if (this == &other) return *this;
    items_ = other.items_;
    return *this;
}

template <class T>
ArraySequence<T>::~ArraySequence() = default;

template <class T>
int ArraySequence<T>::GetLength() const { return items_.GetSize(); }

template <class T>
T ArraySequence<T>::GetFirst() const {
    if (items_.GetSize() == 0) throw IndexOutOfRange("GetFirst on empty sequence");
    return items_.Get(0);
}

template <class T>
T ArraySequence<T>::GetLast() const {
    if (items_.GetSize() == 0) throw IndexOutOfRange("GetLast on empty sequence");
    return items_.Get(items_.GetSize() - 1);
}

template <class T>
T ArraySequence<T>::Get(int index) const { return items_.Get(index); }

template <class T>
Sequence<T>* ArraySequence<T>::GetSubsequence(int start_index, int end_index) const {
    const int n = items_.GetSize();
    if (start_index < 0 || start_index >= n) {
        throw IndexOutOfRange("GetSubsequence: start " + std::to_string(start_index) +
                              " out of [0, " + std::to_string(n) + ")");
    }
    if (end_index < 0 || end_index >= n) {
        throw IndexOutOfRange("GetSubsequence: end " + std::to_string(end_index) +
                              " out of [0, " + std::to_string(n) + ")");
    }
    if (start_index > end_index) {
        throw InvalidArgument("GetSubsequence: start " + std::to_string(start_index) +
                              " greater than end " + std::to_string(end_index));
    }
    ArraySequence<T>* sub = MakeEmpty();
    for (int i = start_index; i <= end_index; ++i) sub->AppendInternal(items_.Get(i));
    return sub;
}

template <class T>
Sequence<T>* ArraySequence<T>::Append(const T& item) {
    ArraySequence<T>* target = Instance();
    target->AppendInternal(item);
    return target;
}

template <class T>
Sequence<T>* ArraySequence<T>::Prepend(const T& item) {
    ArraySequence<T>* target = Instance();
    target->PrependInternal(item);
    return target;
}

template <class T>
Sequence<T>* ArraySequence<T>::InsertAt(const T& item, int index) {
    ArraySequence<T>* target = Instance();
    target->InsertAtInternal(item, index);
    return target;
}

template <class T>
Sequence<T>* ArraySequence<T>::Set(int index, const T& value) {
    ArraySequence<T>* target = Instance();
    target->items_.Set(index, value);
    return target;
}

template <class T>
Sequence<T>* ArraySequence<T>::Concat(const Sequence<T>* other) {
    if (other == nullptr) throw InvalidArgument("Concat: other sequence is null");
    ArraySequence<T>* target = Instance();
    const int n = other->GetLength();
    for (int i = 0; i < n; ++i) target->AppendInternal(other->Get(i));
    return target;
}

template <class T>
IEnumerator<T>* ArraySequence<T>::GetEnumerator() const {
    return new ArrayEnumerator(&items_);
}

template <class T>
void ArraySequence<T>::AppendInternal(const T& item) {
    items_.Resize(items_.GetSize() + 1);
    items_.Set(items_.GetSize() - 1, item);
}

template <class T>
void ArraySequence<T>::PrependInternal(const T& item) {
    const int old_size = items_.GetSize();
    items_.Resize(old_size + 1);
    for (int i = old_size; i > 0; --i) items_.Set(i, items_.Get(i - 1));
    items_.Set(0, item);
}

template <class T>
void ArraySequence<T>::InsertAtInternal(const T& item, int index) {
    const int old_size = items_.GetSize();
    if (index < 0 || index > old_size) {
        throw IndexOutOfRange("InsertAt: index " + std::to_string(index) +
                              " out of [0, " + std::to_string(old_size) + "]");
    }
    items_.Resize(old_size + 1);
    for (int i = old_size; i > index; --i) items_.Set(i, items_.Get(i - 1));
    items_.Set(index, item);
}

// MutableArraySequence
template <class T>
MutableArraySequence<T>::MutableArraySequence() : ArraySequence<T>() {}

template <class T>
MutableArraySequence<T>::MutableArraySequence(const T* items, int count)
    : ArraySequence<T>(items, count) {}

template <class T>
MutableArraySequence<T>::MutableArraySequence(const DynamicArray<T>& source)
    : ArraySequence<T>(source) {}

template <class T>
MutableArraySequence<T>::MutableArraySequence(const MutableArraySequence<T>& other)
    : ArraySequence<T>(other) {}

template <class T>
Sequence<T>* MutableArraySequence<T>::Clone() const {
    return new MutableArraySequence<T>(*this);
}

template <class T>
ArraySequence<T>* MutableArraySequence<T>::Instance() { return this; }

template <class T>
ArraySequence<T>* MutableArraySequence<T>::MakeEmpty() const {
    return new MutableArraySequence<T>();
}

// ImmutableArraySequence
template <class T>
ImmutableArraySequence<T>::ImmutableArraySequence() : ArraySequence<T>() {}

template <class T>
ImmutableArraySequence<T>::ImmutableArraySequence(const T* items, int count)
    : ArraySequence<T>(items, count) {}

template <class T>
ImmutableArraySequence<T>::ImmutableArraySequence(const DynamicArray<T>& source)
    : ArraySequence<T>(source) {}

template <class T>
ImmutableArraySequence<T>::ImmutableArraySequence(const ImmutableArraySequence<T>& other)
    : ArraySequence<T>(other) {}

template <class T>
Sequence<T>* ImmutableArraySequence<T>::Clone() const {
    return new ImmutableArraySequence<T>(*this);
}

template <class T>
ArraySequence<T>* ImmutableArraySequence<T>::Instance() {
    return static_cast<ArraySequence<T>*>(this->Clone());
}

template <class T>
ArraySequence<T>* ImmutableArraySequence<T>::MakeEmpty() const {
    return new ImmutableArraySequence<T>();
}

}  // namespace lab2

#endif
