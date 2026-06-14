#ifndef LAB2_LINKED_LIST_TPP
#define LAB2_LINKED_LIST_TPP

#include <algorithm>
#include <string>

namespace lab2 {

template <class T>
LinkedList<T>::LinkedList() : head_(nullptr), tail_(nullptr), length_(0) {}

template <class T>
LinkedList<T>::LinkedList(const T* items, int count)
    : head_(nullptr), tail_(nullptr), length_(0) {
    if (count < 0) {
        throw InvalidArgument("LinkedList: count must be non-negative, got " +
                              std::to_string(count));
    }
    if (count > 0 && items == nullptr) {
        throw InvalidArgument("LinkedList: source pointer is null");
    }
    for (int i = 0; i < count; ++i) Append(items[i]);
}

template <class T>
LinkedList<T>::LinkedList(const LinkedList<T>& other)
    : head_(nullptr), tail_(nullptr), length_(0) {
    for (Node* cur = other.head_; cur != nullptr; cur = cur->next) Append(cur->value);
}

template <class T>
LinkedList<T>& LinkedList<T>::operator=(const LinkedList<T>& other) {
    if (this == &other) return *this;
    LinkedList<T> tmp(other);
    Swap(tmp);
    return *this;
}

template <class T>
LinkedList<T>::~LinkedList() { Clear(); }

template <class T>
int LinkedList<T>::GetLength() const { return length_; }

template <class T>
T& LinkedList<T>::GetFirst() {
    if (head_ == nullptr) throw IndexOutOfRange("GetFirst on empty list");
    return head_->value;
}

template <class T>
const T& LinkedList<T>::GetFirst() const {
    if (head_ == nullptr) throw IndexOutOfRange("GetFirst on empty list");
    return head_->value;
}

template <class T>
T& LinkedList<T>::GetLast() {
    if (tail_ == nullptr) throw IndexOutOfRange("GetLast on empty list");
    return tail_->value;
}

template <class T>
const T& LinkedList<T>::GetLast() const {
    if (tail_ == nullptr) throw IndexOutOfRange("GetLast on empty list");
    return tail_->value;
}

template <class T>
T& LinkedList<T>::Get(int index) { return NodeAt(index)->value; }

template <class T>
const T& LinkedList<T>::Get(int index) const { return NodeAt(index)->value; }

template <class T>
void LinkedList<T>::Set(int index, const T& value) { NodeAt(index)->value = value; }

template <class T>
LinkedList<T>* LinkedList<T>::GetSubList(int start_index, int end_index) const {
    EnsureIndexValid(start_index);
    EnsureIndexValid(end_index);
    if (start_index > end_index) {
        throw InvalidArgument("GetSubList: start " + std::to_string(start_index) +
                              " greater than end " + std::to_string(end_index));
    }
    LinkedList<T>* result = new LinkedList<T>();
    Node* cur = head_;
    for (int i = 0; i < start_index; ++i) cur = cur->next;
    for (int i = start_index; i <= end_index; ++i) {
        result->Append(cur->value);
        cur = cur->next;
    }
    return result;
}

template <class T>
void LinkedList<T>::Append(const T& item) {
    Node* node = new Node(item);
    if (tail_ == nullptr) {
        head_ = tail_ = node;
    } else {
        tail_->next = node;
        tail_ = node;
    }
    ++length_;
}

template <class T>
void LinkedList<T>::Prepend(const T& item) {
    Node* node = new Node(item);
    node->next = head_;
    head_ = node;
    if (tail_ == nullptr) tail_ = node;
    ++length_;
}

template <class T>
void LinkedList<T>::InsertAt(const T& item, int index) {
    if (index < 0 || index > length_) {
        throw IndexOutOfRange("InsertAt: index " + std::to_string(index) +
                              " out of [0, " + std::to_string(length_) + "]");
    }
    if (index == 0) { Prepend(item); return; }
    if (index == length_) { Append(item); return; }
    Node* prev = head_;
    for (int i = 0; i < index - 1; ++i) prev = prev->next;
    Node* node = new Node(item);
    node->next = prev->next;
    prev->next = node;
    ++length_;
}

template <class T>
LinkedList<T>* LinkedList<T>::Concat(const LinkedList<T>* other) const {
    if (other == nullptr) throw InvalidArgument("Concat: other list is null");
    LinkedList<T>* result = new LinkedList<T>(*this);
    for (Node* cur = other->head_; cur != nullptr; cur = cur->next) {
        result->Append(cur->value);
    }
    return result;
}

template <class T>
typename LinkedList<T>::Node* LinkedList<T>::NodeAt(int index) const {
    EnsureIndexValid(index);
    Node* cur = head_;
    for (int i = 0; i < index; ++i) cur = cur->next;
    return cur;
}

template <class T>
void LinkedList<T>::EnsureIndexValid(int index) const {
    if (index < 0 || index >= length_) {
        throw IndexOutOfRange("index " + std::to_string(index) + " out of [0, " +
                              std::to_string(length_) + ")");
    }
}

template <class T>
void LinkedList<T>::Clear() {
    Node* cur = head_;
    while (cur != nullptr) {
        Node* next = cur->next;
        delete cur;
        cur = next;
    }
    head_ = tail_ = nullptr;
    length_ = 0;
}

template <class T>
void LinkedList<T>::Swap(LinkedList<T>& other) {
    std::swap(head_, other.head_);
    std::swap(tail_, other.tail_);
    std::swap(length_, other.length_);
}

}  // namespace lab2

#endif
