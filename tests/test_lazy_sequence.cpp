#include "../include/errors.h"
#include "../include/lazy_sequence.h"
#include "../include/ordinal.h"
#include "test_framework.h"

#include <cstdio>
#include <functional>

namespace lab4 {
namespace tests {

static int FibRule(const lab2::DynamicArray<int>& materialized) {
    int size = materialized.GetSize();
    return materialized.Get(size - 1) + materialized.Get(size - 2);
}

static int NatRule(const lab2::DynamicArray<int>& materialized) {
    return materialized.GetSize();
}

static int EvenRule(const lab2::DynamicArray<int>& materialized) {
    return materialized.Get(materialized.GetSize() - 1) + 2;
}

static int OddRule(const lab2::DynamicArray<int>& materialized) {
    return materialized.Get(materialized.GetSize() - 1) + 2;
}

void TestLazySequence() {
    std::printf("\n-- LazySequence --\n");

    {
        LazySequence<int> ls;
        T_ASSERT("empty length 0", ls.GetLength() == Cardinal::Finite(0));
        T_ASSERT("empty mat 0", ls.GetMaterializedCount() == 0);
        T_ASSERT_THROWS("empty GetFirst throws", IndexOutOfRange, ls.GetFirst());
    }

    {
        int arr[] = {10, 20, 30, 40, 50};
        LazySequence<int> ls(arr, 5);
        T_ASSERT_EQ("arr length", ls.GetLength().FiniteValue(), 5u);
        T_ASSERT_EQ("arr Get(0)", ls.Get(0), 10);
        T_ASSERT_EQ("arr Get(4)", ls.Get(4), 50);
        T_ASSERT_EQ("arr GetFirst", ls.GetFirst(), 10);
        T_ASSERT_EQ("arr GetLast", ls.GetLast(), 50);
        T_ASSERT_THROWS("arr out of range", IndexOutOfRange, ls.Get(5));
    }

    {
        int seed[] = {1, 1};
        LazySequence<int> fib(FibRule, seed, 2);
        T_ASSERT("fib is infinite", fib.GetLength().IsOmega());
        T_ASSERT_EQ("fib[0]=1", fib.Get(0), 1);
        T_ASSERT_EQ("fib[1]=1", fib.Get(1), 1);
        T_ASSERT_EQ("fib[2]=2", fib.Get(2), 2);
        T_ASSERT_EQ("fib[3]=3", fib.Get(3), 3);
        T_ASSERT_EQ("fib[4]=5", fib.Get(4), 5);
        T_ASSERT_EQ("fib[6]=13", fib.Get(6), 13);
        T_ASSERT_EQ("fib[9]=55", fib.Get(9), 55);
        T_ASSERT_EQ("fib materialized 10", fib.GetMaterializedCount(), 10u);
    }

    {
        int seed[] = {0};
        LazySequence<int> nats(NatRule, seed, 1);
        T_ASSERT_EQ("nat[0]=0", nats.Get(0), 0);
        T_ASSERT_EQ("nat[5]=5", nats.Get(5), 5);
        T_ASSERT_EQ("nat[99]=99", nats.Get(99), 99);
    }

    {
        int arr[] = {1, 2, 3};
        LazySequence<int> ls(arr, 3);
        ls.InsertAt(99, 1);
        T_ASSERT_EQ("insert length", ls.GetLength().FiniteValue(), 4u);
        T_ASSERT_EQ("insert Get(0)", ls.Get(0), 1);
        T_ASSERT_EQ("insert Get(1)", ls.Get(1), 99);
        T_ASSERT_EQ("insert Get(2)", ls.Get(2), 2);
        T_ASSERT_EQ("insert Get(3)", ls.Get(3), 3);
    }

    {
        int arr[] = {2, 3};
        LazySequence<int> ls(arr, 2);
        ls.Prepend(1);
        T_ASSERT_EQ("prepend Get(0)", ls.Get(0), 1);
        T_ASSERT_EQ("prepend Get(1)", ls.Get(1), 2);
        T_ASSERT_EQ("prepend len", ls.GetLength().FiniteValue(), 3u);
    }

    {
        int arr[] = {1, 2};
        LazySequence<int> ls(arr, 2);
        ls.Append(3);
        T_ASSERT_EQ("append len", ls.GetLength().FiniteValue(), 3u);
        T_ASSERT_EQ("append Get(2)", ls.Get(2), 3);
    }

    {
        int arr[] = {10, 20, 30, 40, 50};
        LazySequence<int> ls(arr, 5);
        auto* sub = ls.GetSubsequence(1, 3);
        T_ASSERT_EQ("sub len", sub->GetLength().FiniteValue(), 3u);
        T_ASSERT_EQ("sub Get(0)", sub->Get(0), 20);
        T_ASSERT_EQ("sub Get(2)", sub->Get(2), 40);
        delete sub;
    }

    {
        int arr[] = {1, 2, 3, 4};
        LazySequence<int> ls(arr, 4);
        auto* doubled = ls.Map<int>([](const int& x) { return x * 2; });
        T_ASSERT_EQ("map len", doubled->GetLength().FiniteValue(), 4u);
        T_ASSERT_EQ("map Get(0)", doubled->Get(0), 2);
        T_ASSERT_EQ("map Get(3)", doubled->Get(3), 8);
        delete doubled;
    }

    {
        int arr[] = {-3, -1, 0, 2, 5};
        LazySequence<int> ls(arr, 5);
        auto* pos = ls.Where([](const int& x) { return x > 0; });
        T_ASSERT_EQ("where len", pos->GetLength().FiniteValue(), 2u);
        T_ASSERT_EQ("where Get(0)", pos->Get(0), 2);
        T_ASSERT_EQ("where Get(1)", pos->Get(1), 5);
        delete pos;
    }

    {
        int arr[] = {1, 2, 3, 4, 5};
        LazySequence<int> ls(arr, 5);
        int sum = ls.Reduce<int>([](const int& acc, const int& x) { return acc + x; }, 0, 5);
        T_ASSERT_EQ("reduce sum", sum, 15);
    }

    {
        int seed[] = {0};
        LazySequence<int> nats(NatRule, seed, 1);
        auto* squares = nats.Map<int>([](const int& x) { return x * x; });
        T_ASSERT_EQ("inf map[0]", squares->Get(0), 0);
        T_ASSERT_EQ("inf map[3]", squares->Get(3), 9);
        T_ASSERT_EQ("inf map[5]", squares->Get(5), 25);
        T_ASSERT("inf map infinite", squares->GetLength().IsOmega());
        delete squares;
    }

    {
        int a[] = {1, 2};
        int b[] = {3, 4};
        LazySequence<int> la(a, 2);
        LazySequence<int> lb(b, 2);
        la.Concat(&lb);
        T_ASSERT_EQ("concat len", la.GetLength().FiniteValue(), 4u);
        T_ASSERT_EQ("concat Get(0)", la.Get(0), 1);
        T_ASSERT_EQ("concat Get(3)", la.Get(3), 4);
    }

    {
        int even_seed[] = {0};
        int odd_seed[] = {1};
        LazySequence<int> evens(EvenRule, even_seed, 1);
        LazySequence<int> odds(OddRule, odd_seed, 1);

        evens.Append(10);
        evens.Concat(&odds);

        auto w = Ordinal::Omega();
        T_ASSERT_EQ("even concat Get(5)", evens.Get(5), 10);
        T_ASSERT_EQ("even concat Get(w)", evens.Get(w), 10);
        T_ASSERT_EQ("even concat Get(w+1)", evens.Get(w + Ordinal::Finite(1)), 1);
        T_ASSERT_EQ("even concat Get(w+5)", evens.Get(w + Ordinal::Finite(5)), 9);
    }

    {
        int even_seed[] = {0};
        int odd_seed[] = {1};
        LazySequence<int> evens(EvenRule, even_seed, 1);
        LazySequence<int> odds(OddRule, odd_seed, 1);
        evens.Concat(&odds);

        auto w = Ordinal::Omega();
        T_ASSERT_EQ("inf concat finite Get(5)", evens.Get(5), 10);
        T_ASSERT_EQ("inf concat finite Get(w)", evens.Get(w), 1);
        T_ASSERT_EQ("inf concat finite Get(w+4)", evens.Get(w + Ordinal::Finite(4)), 9);
    }

    {
        int finite_tail[] = {100, 200};
        LazySequence<int> finite(finite_tail, 2);
        int seed[] = {0};
        LazySequence<int> nats(NatRule, seed, 1);
        finite.Concat(&nats);

        T_ASSERT("finite+inf is infinite", finite.GetLength().IsOmega());
        T_ASSERT_EQ("finite+inf Get(0)", finite.Get(0), 100);
        T_ASSERT_EQ("finite+inf Get(1)", finite.Get(1), 200);
        T_ASSERT_EQ("finite+inf Get(2)", finite.Get(2), 0);
        T_ASSERT_EQ("finite+inf Get(5)", finite.Get(5), 3);
    }
}

}  // namespace tests
}  // namespace lab4
