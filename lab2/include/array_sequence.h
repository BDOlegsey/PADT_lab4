#ifndef LAB2_ARRAY_SEQUENCE_H
#define LAB2_ARRAY_SEQUENCE_H

#include "dynamic_array.h"
#include "sequence.h"

namespace lab2 {

template <class T>
class ArraySequence : public Sequence<T> {
public:
    ArraySequence();
    ArraySequence(const T* items, int count);
    explicit ArraySequence(const DynamicArray<T>& source);
    ArraySequence(const ArraySequence<T>& other);
    ArraySequence<T>& operator=(const ArraySequence<T>& other);
    ~ArraySequence() override;

    int GetLength() const override;

    T GetFirst() const override;
    T GetLast() const override;
    T Get(int index) const override;

    Sequence<T>* GetSubsequence(int start_index, int end_index) const override;

    Sequence<T>* Append(const T& item) override;
    Sequence<T>* Prepend(const T& item) override;
    Sequence<T>* InsertAt(const T& item, int index) override;
    Sequence<T>* Set(int index, const T& value) override;
    Sequence<T>* Concat(const Sequence<T>* other) override;

    IEnumerator<T>* GetEnumerator() const override;

protected:
    virtual ArraySequence<T>* Instance() = 0;

    virtual ArraySequence<T>* MakeEmpty() const = 0;

    void AppendInternal(const T& item);
    void PrependInternal(const T& item);
    void InsertAtInternal(const T& item, int index);

    DynamicArray<T> items_;

private:
    class ArrayEnumerator;
};

template <class T>
class MutableArraySequence : public ArraySequence<T> {
public:
    MutableArraySequence();
    MutableArraySequence(const T* items, int count);
    explicit MutableArraySequence(const DynamicArray<T>& source);
    MutableArraySequence(const MutableArraySequence<T>& other);

    Sequence<T>* Clone() const override;

protected:
    ArraySequence<T>* Instance() override;
    ArraySequence<T>* MakeEmpty() const override;
};

template <class T>
class ImmutableArraySequence : public ArraySequence<T> {
public:
    ImmutableArraySequence();
    ImmutableArraySequence(const T* items, int count);
    explicit ImmutableArraySequence(const DynamicArray<T>& source);
    ImmutableArraySequence(const ImmutableArraySequence<T>& other);

    Sequence<T>* Clone() const override;

protected:
    ArraySequence<T>* Instance() override;
    ArraySequence<T>* MakeEmpty() const override;
};

}  // namespace lab2

#include "array_sequence.tpp"

#endif
