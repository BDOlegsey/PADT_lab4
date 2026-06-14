#ifndef LAB2_IENUMERATOR_H
#define LAB2_IENUMERATOR_H

namespace lab2 {

template <class T>
class IEnumerator {
public:
    virtual ~IEnumerator() = default;
    virtual bool MoveNext() = 0;
    virtual T Current() const = 0;
    virtual void Reset() = 0;
};

template <class T>
class IEnumerable {
public:
    virtual ~IEnumerable() = default;
    virtual IEnumerator<T>* GetEnumerator() const = 0;
};

}  // namespace lab2

#endif
