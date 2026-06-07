#include "../include/errors.h"
#include "test_framework.h"
#include "../include/lazy_sequence.h"
#include <cstdio>
#include <functional>

namespace lab4 {
namespace tests {

void TestLazySequence() {
    std::printf("\n-- LazySequence --\n");

    // empty
    {
        LazySequence<int> ls;
        T_ASSERT("empty length 0",   ls.GetLength() == Cardinal::Finite(0));
        T_ASSERT("empty mat 0",      ls.GetMaterializedCount() == 0);
        T_ASSERT_THROWS("empty GetFirst throws", IndexOutOfRange, ls.GetFirst());
    }

    // from array
    {
        int arr[] = {10, 20, 30, 40, 50};
        LazySequence<int> ls(arr, 5);
        T_ASSERT_EQ("arr length",    ls.GetLength().FiniteValue(), 5u);
        T_ASSERT_EQ("arr Get(0)",    ls.Get(0), 10);
        T_ASSERT_EQ("arr Get(4)",    ls.Get(4), 50);
        T_ASSERT_EQ("arr GetFirst",  ls.GetFirst(), 10);
        T_ASSERT_EQ("arr GetLast",   ls.GetLast(), 50);
        T_ASSERT_THROWS("arr out of range", IndexOutOfRange, ls.Get(5));
    }

    // rule-based (Fibonacci)
    {
        int seed[] = {1, 1};
        LazySequence<int> fib(
            [](const std::vector<int>& m) -> int {
                size_t n = m.size();
                return m[n-1] + m[n-2];
            },
            seed, 2
        );
        T_ASSERT("fib is infinite",  fib.GetLength().IsOmega());
        T_ASSERT_EQ("fib[0]=1",      fib.Get(0), 1);
        T_ASSERT_EQ("fib[1]=1",      fib.Get(1), 1);
        T_ASSERT_EQ("fib[2]=2",      fib.Get(2), 2);
        T_ASSERT_EQ("fib[3]=3",      fib.Get(3), 3);
        T_ASSERT_EQ("fib[4]=5",      fib.Get(4), 5);
        T_ASSERT_EQ("fib[6]=13",     fib.Get(6), 13);
        T_ASSERT_EQ("fib[9]=55",     fib.Get(9), 55);
        T_ASSERT_EQ("fib materialized 10", fib.GetMaterializedCount(), 10u);
    }

    // rule-based (naturals: 0,1,2,...)
    {
        int seed[] = {0};
        LazySequence<int> nats(
            [](const std::vector<int>& m) -> int { return (int)m.size(); },
            seed, 1
        );
        T_ASSERT_EQ("nat[0]=0", nats.Get(0), 0);
        T_ASSERT_EQ("nat[5]=5", nats.Get(5), 5);
        T_ASSERT_EQ("nat[99]=99", nats.Get(99), 99);
    }

    // InsertAt in finite sequence
    {
        int arr[] = {1, 2, 3};
        LazySequence<int> ls(arr, 3);
        ls.InsertAt(99, 1);
        T_ASSERT_EQ("insert length",   ls.GetLength().FiniteValue(), 4u);
        T_ASSERT_EQ("insert Get(0)",   ls.Get(0), 1);
        T_ASSERT_EQ("insert Get(1)",   ls.Get(1), 99);
        T_ASSERT_EQ("insert Get(2)",   ls.Get(2), 2);
        T_ASSERT_EQ("insert Get(3)",   ls.Get(3), 3);
    }

    // Prepend
    {
        int arr[] = {2, 3};
        LazySequence<int> ls(arr, 2);
        ls.Prepend(1);
        T_ASSERT_EQ("prepend Get(0)", ls.Get(0), 1);
        T_ASSERT_EQ("prepend Get(1)", ls.Get(1), 2);
        T_ASSERT_EQ("prepend len",    ls.GetLength().FiniteValue(), 3u);
    }

    // Append
    {
        int arr[] = {1, 2};
        LazySequence<int> ls(arr, 2);
        ls.Append(3);
        T_ASSERT_EQ("append len",    ls.GetLength().FiniteValue(), 3u);
        T_ASSERT_EQ("append Get(2)", ls.Get(2), 3);
    }

    // GetSubsequence
    {
        int arr[] = {10, 20, 30, 40, 50};
        LazySequence<int> ls(arr, 5);
        auto* sub = ls.GetSubsequence(1, 3);
        T_ASSERT_EQ("sub len",    sub->GetLength().FiniteValue(), 3u);
        T_ASSERT_EQ("sub Get(0)", sub->Get(0), 20);
        T_ASSERT_EQ("sub Get(2)", sub->Get(2), 40);
        delete sub;
    }

    // Map
    {
        int arr[] = {1, 2, 3, 4};
        LazySequence<int> ls(arr, 4);
        auto* doubled = ls.Map<int>([](const int& x) { return x * 2; });
        T_ASSERT_EQ("map len",      doubled->GetLength().FiniteValue(), 4u);
        T_ASSERT_EQ("map Get(0)",   doubled->Get(0), 2);
        T_ASSERT_EQ("map Get(3)",   doubled->Get(3), 8);
        delete doubled;
    }

    // Where
    {
        int arr[] = {-3, -1, 0, 2, 5};
        LazySequence<int> ls(arr, 5);
        auto* pos = ls.Where([](const int& x) { return x > 0; });
        T_ASSERT_EQ("where len",    pos->GetLength().FiniteValue(), 2u);
        T_ASSERT_EQ("where Get(0)", pos->Get(0), 2);
        T_ASSERT_EQ("where Get(1)", pos->Get(1), 5);
        delete pos;
    }

    // Reduce
    {
        int arr[] = {1, 2, 3, 4, 5};
        LazySequence<int> ls(arr, 5);
        int sum = ls.Reduce<int>([](const int& acc, const int& x) { return acc + x; }, 0, 5);
        T_ASSERT_EQ("reduce sum", sum, 15);
    }

    // Map on infinite (naturals)
    {
        int seed[] = {0};
        LazySequence<int> nats(
            [](const std::vector<int>& m) -> int { return (int)m.size(); },
            seed, 1
        );
        auto* squares = nats.Map<int>([](const int& x) { return x * x; });
        T_ASSERT_EQ("inf map[0]", squares->Get(0), 0);
        T_ASSERT_EQ("inf map[3]", squares->Get(3), 9);
        T_ASSERT_EQ("inf map[5]", squares->Get(5), 25);
        T_ASSERT("inf map infinite", squares->GetLength().IsOmega());
        delete squares;
    }

    // Concat two finite
    {
        int a[] = {1, 2};
        int b[] = {3, 4};
        LazySequence<int> la(a, 2);
        LazySequence<int> lb(b, 2);
        la.Concat(&lb);
        T_ASSERT_EQ("concat len", la.GetLength().FiniteValue(), 4u);
    }
}

}  // namespace tests
}  // namespace lab4
