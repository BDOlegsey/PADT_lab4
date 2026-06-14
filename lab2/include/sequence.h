#ifndef LAB2_SEQUENCE_H
#define LAB2_SEQUENCE_H

#include "errors.h"
#include "i_enumerator.h"
#include "option.h"

namespace lab2 {

template <class T>
class Sequence : public IEnumerable<T> {
public:
    using Predicate = bool (*)(const T&);

    virtual ~Sequence() = default;

    virtual int GetLength() const = 0;

    virtual T GetFirst() const = 0;
    virtual T GetLast() const = 0;
    virtual T Get(int index) const = 0;

    virtual Sequence<T>* GetSubsequence(int start_index, int end_index) const = 0;

    virtual Sequence<T>* Append(const T& item) = 0;
    virtual Sequence<T>* Prepend(const T& item) = 0;
    virtual Sequence<T>* InsertAt(const T& item, int index) = 0;
    virtual Sequence<T>* Set(int index, const T& value) = 0;
    virtual Sequence<T>* Concat(const Sequence<T>* other) = 0;

    virtual Sequence<T>* Clone() const = 0;

    Option<T> TryGetFirst(Predicate predicate = nullptr) const;
    Option<T> TryGetLast(Predicate predicate = nullptr) const;

    T operator[](int index) const { return Get(index); }
};

}  // namespace lab2

#include "sequence.tpp"

#endif
