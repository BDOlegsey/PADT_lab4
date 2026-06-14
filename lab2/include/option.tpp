#ifndef LAB2_OPTION_TPP
#define LAB2_OPTION_TPP

namespace lab2 {

template <class T>
Option<T>::Option() : has_value_(false) {}

template <class T>
Option<T> Option<T>::Some(const T& value) {
    Option<T> result;
    new (&result.storage_) T(value);
    result.has_value_ = true;
    return result;
}

template <class T>
Option<T> Option<T>::None() {
    return Option<T>();
}

template <class T>
Option<T>::Option(const Option<T>& other) : has_value_(false) {
    if (other.has_value_) {
        new (&storage_) T(other.Value());
        has_value_ = true;
    }
}

template <class T>
Option<T>& Option<T>::operator=(const Option<T>& other) {
    if (this == &other) return *this;
    Reset();
    if (other.has_value_) {
        new (&storage_) T(other.Value());
        has_value_ = true;
    }
    return *this;
}

template <class T>
Option<T>::~Option() { Reset(); }

template <class T>
bool Option<T>::HasValue() const { return has_value_; }

template <class T>
bool Option<T>::IsNone() const { return !has_value_; }

template <class T>
const T& Option<T>::Value() const {
    if (!has_value_) throw NoneValueAccess();
    return *reinterpret_cast<const T*>(&storage_);
}

template <class T>
T Option<T>::ValueOr(const T& fallback) const {
    return has_value_ ? Value() : fallback;
}

template <class T>
void Option<T>::Reset() {
    if (has_value_) {
        reinterpret_cast<T*>(&storage_)->~T();
        has_value_ = false;
    }
}

}  // namespace lab2

#endif
