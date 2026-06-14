#ifndef LAB2_LINKED_LIST_H
#define LAB2_LINKED_LIST_H

#include "errors.h"

namespace lab2 {

template <class T>
class LinkedList {
public:
    struct Node {
        T value;
        Node* next;
        explicit Node(const T& v) : value(v), next(nullptr) {}
    };

    LinkedList();
    LinkedList(const T* items, int count);
    LinkedList(const LinkedList<T>& other);
    LinkedList<T>& operator=(const LinkedList<T>& other);
    ~LinkedList();

    int GetLength() const;

    T& GetFirst();
    const T& GetFirst() const;

    T& GetLast();
    const T& GetLast() const;

    T& Get(int index);
    const T& Get(int index) const;

    void Set(int index, const T& value);

    LinkedList<T>* GetSubList(int start_index, int end_index) const;

    void Append(const T& item);
    void Prepend(const T& item);
    void InsertAt(const T& item, int index);

    LinkedList<T>* Concat(const LinkedList<T>* other) const;

    const Node* HeadNode() const { return head_; }

private:
    Node* NodeAt(int index) const;
    void EnsureIndexValid(int index) const;
    void Clear();
    void Swap(LinkedList<T>& other);

    Node* head_;
    Node* tail_;
    int length_;
};

}  // namespace lab2

#include "linked_list.tpp"

#endif
