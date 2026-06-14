#ifndef LAB2_OPTION_H
#define LAB2_OPTION_H

#include "errors.h"

namespace lab2 {

template <class T>
class Option {
public:
    Option();

    static Option<T> Some(const T& value);
    static Option<T> None();

    Option(const Option<T>& other);
    Option<T>& operator=(const Option<T>& other);
    ~Option();

    bool HasValue() const;
    bool IsNone() const;

    const T& Value() const;
    T ValueOr(const T& fallback) const;

private:
    void Reset();

    bool has_value_;
    alignas(T) unsigned char storage_[sizeof(T)];
};

}  // namespace lab2

#include "option.tpp"

#endif
