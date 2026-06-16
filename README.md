# Lab 4 — Lazy Sequences & Ordinals
## Variant 2: Algebra of Lazy Lists and Ordinal Numbers

### Run
```
mkdir build 2>nul || true && g++ -std=c++17 -Wall -Wextra -Iinclude -Ilab2/include src/main.cpp src/console_ui.cpp tests/test_runner.cpp tests/test_framework.cpp tests/test_cardinal.cpp tests/test_ordinal.cpp tests/test_lazy_sequence.cpp tests/test_stream.cpp include/cardinal.cpp include/ordinal.cpp -o build/app
./build/app
```

### Run tests
```
mkdir build 2>nul || true && g++ -std=c++17 -Wall -Wextra -Iinclude -Ilab2/include tests/test_main.cpp tests/test_runner.cpp tests/test_framework.cpp tests/test_cardinal.cpp tests/test_ordinal.cpp tests/test_lazy_sequence.cpp tests/test_stream.cpp include/cardinal.cpp include/ordinal.cpp -o build/test_bin
./build/test_bin
```
