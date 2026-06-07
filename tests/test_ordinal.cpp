#include "../include/errors.h"
#include "test_framework.h"
#include "../include/ordinal.h"
#include <cstdio>

namespace lab4 {
namespace tests {

void TestOrdinal() {
    std::printf("\n-- Ordinal --\n");

    auto zero  = Ordinal::Zero();
    auto one   = Ordinal::Finite(1);
    auto three = Ordinal::Finite(3);
    auto five  = Ordinal::Finite(5);
    auto w     = Ordinal::Omega();

    // basic predicates
    T_ASSERT("zero is zero",             zero.IsZero());
    T_ASSERT("zero is finite",           zero.IsFinite());
    T_ASSERT("3 is finite",             three.IsFinite());
    T_ASSERT("omega is transfinite",     w.IsTransfinite());
    T_ASSERT_EQ("3 finite value",        three.FiniteValue(), 3u);
    T_ASSERT_THROWS("omega finite val",  InvalidArgument, w.FiniteValue());

    // comparison
    T_ASSERT("0 < 1",                    zero < one);
    T_ASSERT("3 < 5",                    three < five);
    T_ASSERT("5 > 3",                    five > three);
    T_ASSERT("3 < omega",               three < w);
    T_ASSERT("omega > 5",               w > five);
    T_ASSERT("omega == omega",           w == Ordinal::Omega());
    T_ASSERT("3 != 5",                  three != five);

    // addition
    auto r1 = three + five;
    T_ASSERT_EQ("3 + 5 = 8",            r1.FiniteValue(), 8u);

    auto r2 = three + w;
    T_ASSERT("3 + w = w",               r2 == w);

    auto r3 = w + three;
    T_ASSERT("w + 3 is transfinite",    r3.IsTransfinite());
    T_ASSERT("w + 3 > w",               r3 > w);
    T_ASSERT_EQ("w + 3 toString",       r3.ToString(), std::string("w + 3"));

    // multiplication
    auto r4 = three * five;
    T_ASSERT_EQ("3 * 5 = 15",           r4.FiniteValue(), 15u);

    auto r5 = Ordinal::Finite(2) * w;
    T_ASSERT("2 * w = w",               r5 == w);

    auto r6 = w * Ordinal::Finite(2);
    T_ASSERT_EQ("w * 2 toString",       r6.ToString(), std::string("w*2"));

    auto r7 = w * w;
    T_ASSERT_EQ("w * w = w^2",          r7.ToString(), std::string("w^2"));

    // exponentiation
    auto r8 = three.Pow(five);
    T_ASSERT_EQ("3^5 = 243",            r8.FiniteValue(), 243u);

    auto r9 = Ordinal::Finite(2).Pow(w);
    T_ASSERT("2^w = w",                 r9 == w);

    auto r10 = w.Pow(Ordinal::Finite(2));
    T_ASSERT_EQ("w^2 toString",         r10.ToString(), std::string("w^2"));

    auto r11 = w.Pow(w);
    T_ASSERT_EQ("w^w toString",         r11.ToString(), std::string("w^w"));

    // w^2 + w + 1
    auto r12 = r10 + w + one;
    T_ASSERT_EQ("w^2+w+1 toString",     r12.ToString(), std::string("w^2 + w + 1"));

    // zero cases
    T_ASSERT("0 + w = w",               (zero + w) == w);
    T_ASSERT("w + 0 = w",               (w + zero) == w);
    T_ASSERT("0 * w = 0",               (zero * w).IsZero());
    T_ASSERT("w * 0 = 0",               (w * zero).IsZero());
    T_ASSERT_EQ("w^0 = 1",             w.Pow(zero).FiniteValue(), 1u);
    T_ASSERT_EQ("0^0 = 1",             zero.Pow(zero).FiniteValue(), 1u);
}

}  // namespace tests
}  // namespace lab4
