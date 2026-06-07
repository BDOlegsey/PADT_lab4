# Lab 4 — Lazy Sequences & Ordinals
## Variant 2: Algebra of Lazy Lists and Ordinal Numbers

### Build & Run
```
make all        # build app + tests
make test       # run 121 unit tests (100% pass)
make run        # interactive UI
make clean
```

### Project Structure
```
include/
  errors.h          — exceptions: IndexOutOfRange, EmptyContainer, InvalidArgument, NoneValueAccess, EndOfStream
  option.h/.tpp     — Option<T>: Some / None (placement-new, no heap)
  cardinal.h/.cpp   — Cardinal: finite | omega, arithmetic, comparison
  ordinal.h/.cpp    — Ordinal (Cantor normal form): +, *, ^ with full ordinal algebra
  generator.h/.tpp  — Generator<T>: rule engine with memoized pending insert/remove ops
  lazy_sequence.h/.tpp — LazySequence<T>: lazy/infinite sequences with memoization
  stream.h/.tpp     — ReadOnlyStream<T>, WriteOnlyStream<T>

src/
  console_ui.h/.cpp — numeric menu UI
  main.cpp

tests/
  test_framework.h/.cpp — T_ASSERT / T_ASSERT_EQ / T_ASSERT_THROWS macros
  test_runner.cpp       — RunAllTests()
  test_main.cpp         — standalone test binary entry
  test_cardinal.cpp     — 16 tests
  test_ordinal.cpp      — 31 tests
  test_lazy_sequence.cpp — 48 tests
  test_stream.cpp       — 26 tests
```

### Ordinal Algebra
Ordinals are stored in Cantor Normal Form: α = ω^e₁·c₁ + ω^e₂·c₂ + ... (e₁ > e₂ > ...).

Examples:
- ω + 3 = `w + 3`
- ω² = `w^2`
- ω^ω = `w^w`
- 2^ω = ω (correct: sup{2^n} = ω)
- ω^(ω²) = `w^(w^2)`

### LazySequence
- `LazySequence()` — empty finite
- `LazySequence(T*, count)` — from array (finite)
- `LazySequence(rule, seed, k)` — infinite, rule uses k preceding elements
- `Get(i)` — memoized O(1) amortized
- `InsertAt`, `Prepend`, `Append` — pending ops via Generator
- `Map`, `Where`, `Reduce`, `Concat`, `Zip`

### Streams
- `ReadOnlyStream<T>` — from vector/array/string+deserializer, Seek, GoBack, TryRead
- `WriteOnlyStream<T>` — Write, sink to external vector or internal buffer
