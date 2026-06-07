#ifndef LAB4_CONSOLE_UI_H
#define LAB4_CONSOLE_UI_H

#include "../include/lazy_sequence.h"
#include "../include/ordinal.h"
#include "../include/stream.h"

namespace lab4 {

class ConsoleUi {
public:
    ConsoleUi();
    ~ConsoleUi();

    void Run();

private:
    void PrintMenu() const;

    // menu handlers
    void RunTests();
    void MakeLazySequence();
    void ShowSequence();
    void DoAppend();
    void DoPrepend();
    void DoInsertAt();
    void DoGetElement();
    void DoGetSubsequence();
    void DoMap();
    void DoWhere();
    void DoReduce();
    void DoConcat();
    void StreamDemo();
    void OrdinalMenu();

    // helpers
    int  ReadInt(const char* prompt) const;
    bool ReadIntList(const char* prompt, int* out, int max, int* count);
    int  ReadMenuChoice() const;
    void PrintLazySeq(const char* label, LazySequence<int>* seq, int show_count = 10) const;

    LazySequence<int>* seq_a_;
    LazySequence<int>* seq_b_;
};

}  // namespace lab4
#endif
