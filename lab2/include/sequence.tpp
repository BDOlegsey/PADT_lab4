#ifndef LAB2_SEQUENCE_TPP
#define LAB2_SEQUENCE_TPP

namespace lab2 {

template <class T>
Option<T> Sequence<T>::TryGetFirst(Predicate predicate) const {
    const int n = GetLength();
    for (int i = 0; i < n; ++i) {
        const T value = Get(i);
        if (predicate == nullptr || predicate(value)) return Option<T>::Some(value);
    }
    return Option<T>::None();
}

template <class T>
Option<T> Sequence<T>::TryGetLast(Predicate predicate) const {
    const int n = GetLength();
    for (int i = n - 1; i >= 0; --i) {
        const T value = Get(i);
        if (predicate == nullptr || predicate(value)) return Option<T>::Some(value);
    }
    return Option<T>::None();
}

}  // namespace lab2

#endif
