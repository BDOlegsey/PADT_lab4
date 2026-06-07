g++ -std=c++17 -Wall -Wextra \
    -Iinclude -Isrc \
    include/cardinal.cpp \
    include/ordinal.cpp \
    src/console_ui.cpp \
    tests/test_framework.cpp \
    tests/test_runner.cpp \
    tests/test_cardinal.cpp \
    tests/test_ordinal.cpp \
    tests/test_lazy_sequence.cpp \
    tests/test_stream.cpp \
    tests/test_main.cpp \
    -o test_bin
