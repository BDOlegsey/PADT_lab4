#include "test_framework.h"

namespace lab4 {
namespace tests { void RunAllTests(); }
}  // namespace lab4

int main() {
    lab4::tests::RunAllTests();
    return lab4::tests::t_failed == 0 ? 0 : 1;
}
