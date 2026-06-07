#ifndef LAB4_ORDINAL_H
#define LAB4_ORDINAL_H

#include <cstdint>
#include <string>
#include <vector>

namespace lab4 {

class Ordinal {
public:
    struct Term {
        Ordinal* exponent;
        uint64_t coef;
    };

    Ordinal();
    Ordinal(uint64_t n);
    Ordinal(const Ordinal& other);
    Ordinal& operator=(const Ordinal& other);
    ~Ordinal();

    static Ordinal Zero();
    static Ordinal Finite(uint64_t n);
    static Ordinal Omega();

    bool IsZero() const;
    bool IsFinite() const;
    bool IsTransfinite() const;
    uint64_t FiniteValue() const;

    Ordinal operator+(const Ordinal& other) const;
    Ordinal operator*(const Ordinal& other) const;
    Ordinal Pow(const Ordinal& exp) const;

    bool operator==(const Ordinal& other) const;
    bool operator!=(const Ordinal& other) const;
    bool operator<(const Ordinal& other) const;
    bool operator<=(const Ordinal& other) const;
    bool operator>(const Ordinal& other) const;

    std::string ToString() const;

    const std::vector<Term>& Terms() const { return terms_; }

private:
    std::vector<Term> terms_;

    static Ordinal FromTerms(std::vector<Term>&& terms);
    Ordinal IntPow(uint64_t n) const;
    static Ordinal ShiftExpDown(const Ordinal& e);
};

}  // namespace lab4

#endif
