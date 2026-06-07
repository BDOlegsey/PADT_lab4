#include "../include/errors.h"
#include "test_framework.h"
#include "../include/cardinal.h"
#include <cstdio>

namespace lab4 {
namespace tests {

void TestCardinal() {
    std::printf("\n-- Cardinal --\n");

    auto f3  = Cardinal::Finite(3);
    auto f5  = Cardinal::Finite(5);
    auto w   = Cardinal::Omega();

    T_ASSERT("finite is finite",          f3.IsFinite());
    T_ASSERT("omega is omega",            w.IsOmega());
    T_ASSERT_EQ("finite value",           f3.FiniteValue(), 3u);
    T_ASSERT("3 < 5",                     f3 < f5);
    T_ASSERT("5 > 3",                     f5 > f3);
    T_ASSERT("omega > finite",            w > f5);
    T_ASSERT("finite < omega",            f3 < w);
    T_ASSERT("omega not finite",          !w.IsFinite());
    T_ASSERT_EQ("3 + 5 = 8",             (f3 + f5).FiniteValue(), 8u);
    T_ASSERT("3 + omega = omega",         (f3 + w).IsOmega());
    T_ASSERT("omega + 1 = omega",         (w + Cardinal::Finite(1)).IsOmega());
    T_ASSERT_EQ("omega toString",         w.ToString(), std::string("omega"));
    T_ASSERT_EQ("3 toString",             f3.ToString(), std::string("3"));
    T_ASSERT("3 == 3",                    f3 == Cardinal::Finite(3));
    T_ASSERT("3 != 5",                    f3 != f5);
    T_ASSERT_THROWS("finite value of omega throws", InvalidArgument, w.FiniteValue());
}

}  // namespace tests
}  // namespace lab4
