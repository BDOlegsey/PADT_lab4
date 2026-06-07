#ifndef LAB4_ORDINAL_H
#define LAB4_ORDINAL_H

#include <cstdint>
#include <string>
#include <vector>

namespace lab4 {

// Cantor normal form: alpha = sum of w^e_i * c_i, e_1 > e_2 > ... >= 0, c_i > 0
// Each exponent is itself an Ordinal (recursive).
class Ordinal {
public:
    struct Term {
        Ordinal* exponent;  // owned
        uint64_t coef;
    };

    Ordinal();                          // zero
    Ordinal(uint64_t n);               // finite ordinal
    Ordinal(const Ordinal& other);
    Ordinal& operator=(const Ordinal& other);
    ~Ordinal();

    static Ordinal Zero();
    static Ordinal Finite(uint64_t n);
    static Ordinal Omega();             // w^1 * 1

    bool IsZero() const;
    bool IsFinite() const;
    bool IsTransfinite() const;
    uint64_t FiniteValue() const;       // valid only if IsFinite()

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
    std::vector<Term> terms_;  // sorted descending by exponent

    static Ordinal FromTerms(std::vector<Term>&& terms);
    Ordinal IntPow(uint64_t n) const;   // this^n for finite n
    // helper: divide by omega (shift exponents down by 1) for n^gamma formula
    static Ordinal ShiftExpDown(const Ordinal& e);
};

}  // namespace lab4

#endif
