#ifndef LAB2_DYNAMIC_ARRAY_H
#define LAB2_DYNAMIC_ARRAY_H

#include "errors.h"

namespace lab2 {

template <class T>
class DynamicArray {
public:
    DynamicArray();
    explicit DynamicArray(int size);
    DynamicArray(const T* items, int count);
    DynamicArray(const DynamicArray<T>& other);
    DynamicArray<T>& operator=(const DynamicArray<T>& other);
    ~DynamicArray();

    int GetSize() const;
    int GetCapacity() const;

    T& Get(int index);
    const T& Get(int index) const;

    void Set(int index, const T& value);
    void Resize(int new_size);

    T& operator[](int index);
    const T& operator[](int index) const;

private:
    void EnsureIndexValid(int index) const;
    void EnsureCapacity(int min_capacity);
    void Swap(DynamicArray<T>& other);

    T* data_;
    int size_;
    int capacity_;
};

}  // namespace lab2

#include "dynamic_array.tpp"

#endif
