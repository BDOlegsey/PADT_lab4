#ifndef LAB2_SEQUENCE_ALGORITHMS_H
#define LAB2_SEQUENCE_ALGORITHMS_H

#include "array_sequence.h"
#include "list_sequence.h"
#include "sequence.h"

namespace lab2 {

template <class T1, class T2>
struct Pair {
    T1 first;
    T2 second;

    Pair() : first(), second() {}
    Pair(const T1& f, const T2& s) : first(f), second(s) {}

    bool operator==(const Pair<T1, T2>& other) const {
        return first == other.first && second == other.second;
    }
    bool operator!=(const Pair<T1, T2>& other) const { return !(*this == other); }
};

namespace algorithms {

template <class T, class R>
Sequence<R>* Map(const Sequence<T>& source, R (*func)(const T&));

template <class T>
Sequence<T>* Where(const Sequence<T>& source, bool (*predicate)(const T&));

template <class T, class R>
R Reduce(const Sequence<T>& source, R (*func)(const T&, const R&), const R& initial);

template <class T, class R>
Sequence<R>* FlatMap(const Sequence<T>& source, Sequence<R>* (*func)(const T&));

template <class T1, class T2>
Sequence<Pair<T1, T2>>* Zip(const Sequence<T1>& first, const Sequence<T2>& second);

template <class T1, class T2>
Pair<Sequence<T1>*, Sequence<T2>*> Unzip(const Sequence<Pair<T1, T2>>& source);

template <class T>
Sequence<Sequence<T>*>* Split(const Sequence<T>& source, bool (*predicate)(const T&));

template <class T>
Sequence<T>* Slice(const Sequence<T>& source, int index, int length, const Sequence<T>& insert);

}  // namespace algorithms
}  // namespace lab2

#include "sequence_algorithms.tpp"

#endif
