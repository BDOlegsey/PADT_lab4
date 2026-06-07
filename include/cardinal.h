#ifndef LAB4_CARDINAL_H
#define LAB4_CARDINAL_H

#include <cstddef>
#include <string>

namespace lab4 {

// represents natural numbers or omega (first transfinite)
class Cardinal {
public:
    static Cardinal Finite(size_t n);
    static Cardinal Omega();

    bool IsFinite() const;
    bool IsOmega() const;
    size_t FiniteValue() const;  // undefined if omega

    Cardinal operator+(const Cardinal& other) const;
    Cardinal operator+(size_t n) const;
    bool operator==(const Cardinal& other) const;
    bool operator!=(const Cardinal& other) const;
    bool operator<(const Cardinal& other) const;
    bool operator<=(const Cardinal& other) const;
    bool operator>(const Cardinal& other) const;

    std::string ToString() const;

private:
    explicit Cardinal(bool is_omega, size_t value);
    bool is_omega_;
    size_t value_;
};

}  // namespace lab4

#endif
