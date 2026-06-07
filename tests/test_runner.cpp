#include "test_framework.h"
#include <cstdio>

namespace lab4 {
namespace tests {

void TestCardinal();
void TestOrdinal();
void TestLazySequence();
void TestStream();

void RunAllTests() {
    ResetCounters();
    TestCardinal();
    TestOrdinal();
    TestLazySequence();
    TestStream();
    std::printf("\n");
    PrintSummary();
}

}  // namespace tests
}  // namespace lab4
