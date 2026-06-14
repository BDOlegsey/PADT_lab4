#ifndef LAB2_SEQUENCE_ALGORITHMS_TPP
#define LAB2_SEQUENCE_ALGORITHMS_TPP

#include "sequence_algorithms.h"

namespace lab2 {
namespace algorithms {

template <class T, class R>
Sequence<R>* Map(const Sequence<T>& source, R (*func)(const T&)) {
    if (func == nullptr) throw InvalidArgument("map function is null");
    MutableArraySequence<R>* result = new MutableArraySequence<R>();
    IEnumerator<T>* it = source.GetEnumerator();
    while (it->MoveNext()) result->Append(func(it->Current()));
    delete it;
    return result;
}

template <class T>
Sequence<T>* Where(const Sequence<T>& source, bool (*predicate)(const T&)) {
    if (predicate == nullptr) throw InvalidArgument("predicate is null");
    MutableArraySequence<T>* result = new MutableArraySequence<T>();
    IEnumerator<T>* it = source.GetEnumerator();
    while (it->MoveNext()) {
        T item = it->Current();
        if (predicate(item)) result->Append(item);
    }
    delete it;
    return result;
}

template <class T, class R>
R Reduce(const Sequence<T>& source, R (*func)(const T&, const R&), const R& initial) {
    if (func == nullptr) throw InvalidArgument("reduce function is null");
    R acc = initial;
    IEnumerator<T>* it = source.GetEnumerator();
    while (it->MoveNext()) acc = func(it->Current(), acc);
    delete it;
    return acc;
}

template <class T, class R>
Sequence<R>* FlatMap(const Sequence<T>& source, Sequence<R>* (*func)(const T&)) {
    if (func == nullptr) throw InvalidArgument("flat-map function is null");
    MutableArraySequence<R>* result = new MutableArraySequence<R>();
    IEnumerator<T>* outer = source.GetEnumerator();
    while (outer->MoveNext()) {
        Sequence<R>* piece = func(outer->Current());
        if (piece == nullptr) {
            delete outer;
            delete result;
            throw InvalidArgument("flat-map function returned null");
        }
        IEnumerator<R>* inner = piece->GetEnumerator();
        while (inner->MoveNext()) result->Append(inner->Current());
        delete inner;
        delete piece;
    }
    delete outer;
    return result;
}

template <class T1, class T2>
Sequence<Pair<T1, T2>>* Zip(const Sequence<T1>& first, const Sequence<T2>& second) {
    MutableArraySequence<Pair<T1, T2>>* result = new MutableArraySequence<Pair<T1, T2>>();
    IEnumerator<T1>* it1 = first.GetEnumerator();
    IEnumerator<T2>* it2 = second.GetEnumerator();
    while (it1->MoveNext() && it2->MoveNext())
        result->Append(Pair<T1, T2>(it1->Current(), it2->Current()));
    delete it1;
    delete it2;
    return result;
}

template <class T1, class T2>
Pair<Sequence<T1>*, Sequence<T2>*> Unzip(const Sequence<Pair<T1, T2>>& source) {
    MutableArraySequence<T1>* a = new MutableArraySequence<T1>();
    MutableArraySequence<T2>* b = new MutableArraySequence<T2>();
    IEnumerator<Pair<T1, T2>>* it = source.GetEnumerator();
    while (it->MoveNext()) {
        Pair<T1, T2> p = it->Current();
        a->Append(p.first);
        b->Append(p.second);
    }
    delete it;
    return Pair<Sequence<T1>*, Sequence<T2>*>(a, b);
}

template <class T>
Sequence<Sequence<T>*>* Split(const Sequence<T>& source, bool (*predicate)(const T&)) {
    if (predicate == nullptr) throw InvalidArgument("predicate is null");
    MutableArraySequence<Sequence<T>*>* result = new MutableArraySequence<Sequence<T>*>();
    MutableArraySequence<T>* chunk = new MutableArraySequence<T>();
    IEnumerator<T>* it = source.GetEnumerator();
    while (it->MoveNext()) {
        T item = it->Current();
        if (predicate(item)) {
            result->Append(chunk);
            chunk = new MutableArraySequence<T>();
        } else {
            chunk->Append(item);
        }
    }
    delete it;
    result->Append(chunk);
    return result;
}

template <class T>
Sequence<T>* Slice(const Sequence<T>& source, int index, int length, const Sequence<T>& insert) {
    if (length < 0) throw InvalidArgument("slice length must be non-negative");
    int n = source.GetLength();
    int real_index = index < 0 ? n + index : index;
    if (real_index < 0 || real_index > n) throw IndexOutOfRange("slice index out of range");
    int end = real_index + length;
    if (end > n) end = n;

    MutableArraySequence<T>* result = new MutableArraySequence<T>();
    
    IEnumerator<T>* it = source.GetEnumerator();
    int pos = 0;
    while (it->MoveNext() && pos < real_index) {
        result->Append(it->Current());
        ++pos;
    }
    delete it;
    
    IEnumerator<T>* ins = insert.GetEnumerator();
    while (ins->MoveNext()) result->Append(ins->Current());
    delete ins;
    
    it = source.GetEnumerator();
    pos = 0;
    while (it->MoveNext()) {
        if (pos >= end) result->Append(it->Current());
        ++pos;
    }
    delete it;
    return result;
}

}  // namespace algorithms
}  // namespace lab2

#endif
