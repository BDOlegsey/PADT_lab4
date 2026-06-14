#ifndef LAB2_DYNAMIC_ARRAY_TPP
#define LAB2_DYNAMIC_ARRAY_TPP

#include <algorithm>
#include <string>

namespace lab2 {

template <class T>
DynamicArray<T>::DynamicArray() : data_(nullptr), size_(0), capacity_(0) {}

template <class T>
DynamicArray<T>::DynamicArray(int size) : data_(nullptr), size_(0), capacity_(0) {
    if (size < 0) {
        throw InvalidArgument("DynamicArray: size must be non-negative, got " +
                              std::to_string(size));
    }
    size_ = size;
    capacity_ = size_;
    data_ = (capacity_ == 0) ? nullptr : new T[capacity_]();
}

template <class T>
DynamicArray<T>::DynamicArray(const T* items, int count) : data_(nullptr), size_(0), capacity_(0) {
    if (count < 0) {
        throw InvalidArgument("DynamicArray: count must be non-negative, got " +
                              std::to_string(count));
    }
    if (count > 0 && items == nullptr) {
        throw InvalidArgument("DynamicArray: source pointer is null");
    }
    size_ = count;
    capacity_ = size_;
    if (size_ > 0) {
        data_ = new T[capacity_];
        for (int i = 0; i < size_; ++i) data_[i] = items[i];
    }
}

template <class T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& other)
    : data_(nullptr), size_(other.size_), capacity_(other.capacity_) {
    if (capacity_ > 0) {
        data_ = new T[capacity_];
        for (int i = 0; i < size_; ++i) data_[i] = other.data_[i];
    }
}

template <class T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T>& other) {
    if (this == &other) return *this;
    DynamicArray<T> tmp(other);
    Swap(tmp);
    return *this;
}

template <class T>
DynamicArray<T>::~DynamicArray() { delete[] data_; }

template <class T>
int DynamicArray<T>::GetSize() const { return size_; }

template <class T>
int DynamicArray<T>::GetCapacity() const { return capacity_; }

template <class T>
T& DynamicArray<T>::Get(int index) {
    EnsureIndexValid(index);
    return data_[index];
}

template <class T>
const T& DynamicArray<T>::Get(int index) const {
    EnsureIndexValid(index);
    return data_[index];
}

template <class T>
void DynamicArray<T>::Set(int index, const T& value) {
    EnsureIndexValid(index);
    data_[index] = value;
}

template <class T>
void DynamicArray<T>::Resize(int new_size) {
    if (new_size < 0) {
        throw InvalidArgument("DynamicArray: new size must be non-negative, got " +
                              std::to_string(new_size));
    }
    if (new_size == size_) return;
    
    EnsureCapacity(new_size);
    size_ = new_size;
}

template <class T>
T& DynamicArray<T>::operator[](int index) { return Get(index); }

template <class T>
const T& DynamicArray<T>::operator[](int index) const { return Get(index); }

template <class T>
void DynamicArray<T>::EnsureIndexValid(int index) const {
    if (index < 0 || index >= size_) {
        throw IndexOutOfRange("index " + std::to_string(index) + " out of [0, " +
                              std::to_string(size_) + ")");
    }
}

template <class T>
void DynamicArray<T>::Swap(DynamicArray<T>& other) {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}

template <class T>
void DynamicArray<T>::EnsureCapacity(int min_capacity) {
    if (min_capacity <= capacity_) {
        return;
    }
    int new_capacity = capacity_ > 0 ? capacity_ : 1;
    while (new_capacity < min_capacity) {
        new_capacity *= 2;
    }
    T* new_data = new T[new_capacity]();
    for (int i = 0; i < size_; ++i) {
        new_data[i] = data_[i];
    }
    delete[] data_;
    data_ = new_data;
    capacity_ = new_capacity;
}

}  // namespace lab2

#endif
