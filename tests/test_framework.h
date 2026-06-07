#ifndef LAB4_TEST_FRAMEWORK_H
#define LAB4_TEST_FRAMEWORK_H

#include <cmath>
#include <cstdio>

namespace lab4 {
namespace tests {

extern int t_passed;
extern int t_failed;
extern int t_total;

void ResetCounters();
void PrintSummary();

}  // namespace tests
}  // namespace lab4

#define T_ASSERT(name, cond)                                           \
    do {                                                               \
        ::lab4::tests::t_total++;                                      \
        if (cond) {                                                    \
            std::printf("  [+] %s\n", name);                           \
            ::lab4::tests::t_passed++;                                 \
        } else {                                                       \
            std::printf("  [-] %s\n", name);                           \
            ::lab4::tests::t_failed++;                                 \
        }                                                              \
    } while (0)

#define T_ASSERT_EQ(name, a, b)     T_ASSERT(name, (a) == (b))
#define T_ASSERT_NE(name, a, b)     T_ASSERT(name, (a) != (b))
#define T_ASSERT_NULL(name, p)      T_ASSERT(name, (p) == nullptr)
#define T_ASSERT_NOT_NULL(name, p)  T_ASSERT(name, (p) != nullptr)

#define T_ASSERT_THROWS(name, ExType, expr)                            \
    do {                                                               \
        bool t_caught = false;                                         \
        try { expr; } catch (const ExType&) { t_caught = true; }      \
        catch (...) {}                                                 \
        T_ASSERT(name, t_caught);                                      \
    } while (0)

#endif
