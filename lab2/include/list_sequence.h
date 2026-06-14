#ifndef LAB2_LIST_SEQUENCE_H
#define LAB2_LIST_SEQUENCE_H

#include "linked_list.h"
#include "sequence.h"

namespace lab2 {

template <class T>
class ListSequence : public Sequence<T> {
public:
    ListSequence();
    ListSequence(const T* items, int count);
    explicit ListSequence(const LinkedList<T>& source);
    ListSequence(const ListSequence<T>& other);
    ListSequence<T>& operator=(const ListSequence<T>& other);
    ~ListSequence() override;

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
    virtual ListSequence<T>* Instance() = 0;
    virtual ListSequence<T>* MakeEmpty() const = 0;

    LinkedList<T> items_;

private:
    class ListEnumerator;
};

template <class T>
class MutableListSequence : public ListSequence<T> {
public:
    MutableListSequence();
    MutableListSequence(const T* items, int count);
    explicit MutableListSequence(const LinkedList<T>& source);
    MutableListSequence(const MutableListSequence<T>& other);

    Sequence<T>* Clone() const override;

protected:
    ListSequence<T>* Instance() override;
    ListSequence<T>* MakeEmpty() const override;
};

template <class T>
class ImmutableListSequence : public ListSequence<T> {
public:
    ImmutableListSequence();
    ImmutableListSequence(const T* items, int count);
    explicit ImmutableListSequence(const LinkedList<T>& source);
    ImmutableListSequence(const ImmutableListSequence<T>& other);

    Sequence<T>* Clone() const override;

protected:
    ListSequence<T>* Instance() override;
    ListSequence<T>* MakeEmpty() const override;
};

}  // namespace lab2

#include "list_sequence.tpp"

#endif
