#include "console_ui.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace lab4 {

static int FibRule(const std::vector<int>& m) {
    size_t n = m.size();
    return m[n-1] + m[n-2];
}

static int NatRule(const std::vector<int>& m) {
    return (int)m.size();
}

static int ParseInt(const std::string& s) {
    return std::stoi(s);
}

static Ordinal ParseOrdinal(const std::string& s) {
    if (s == "w") return Ordinal::Omega();
    if (s.rfind("w+", 0) == 0) {
        int n = std::stoi(s.substr(2));
        return Ordinal::Omega() + Ordinal::Finite((uint64_t)n);
    }
    if (s.rfind("w*", 0) == 0) {
        int n = std::stoi(s.substr(2));
        return Ordinal::Omega() * Ordinal::Finite((uint64_t)n);
    }
    if (s.rfind("w^", 0) == 0) {
        std::string rest = s.substr(2);
        size_t plus = rest.find('+');
        if (plus == std::string::npos) {
            int k = std::stoi(rest);
            return Ordinal::Omega().Pow(Ordinal::Finite((uint64_t)k));
        }
        int k = std::stoi(rest.substr(0, plus));
        Ordinal tail = ParseOrdinal(rest.substr(plus + 1));
        return Ordinal::Omega().Pow(Ordinal::Finite((uint64_t)k)) + tail;
    }
    // finite
    return Ordinal::Finite((uint64_t)std::stoi(s));
}

namespace tests { void RunAllTests(); }
}  // namespace lab4

namespace lab4 {

ConsoleUi::ConsoleUi() : seq_a_(nullptr), seq_b_(nullptr) {}

ConsoleUi::~ConsoleUi() {
    delete seq_a_;
    delete seq_b_;
}

void ConsoleUi::PrintMenu() const {
    std::printf("\n=== Lab 4 — Lazy Sequences & Ordinals ===\n");
    std::printf(" 1. Run all tests\n");
    std::printf(" 2. Make sequence A (finite from input)\n");
    std::printf(" 3. Make sequence A (infinite Fibonacci)\n");
    std::printf(" 4. Make sequence A (infinite naturals 0,1,2,...)\n");
    std::printf(" 5. Show sequence A (first N elements)\n");
    std::printf(" 6. Append to A\n");
    std::printf(" 7. Prepend to A\n");
    std::printf(" 8. InsertAt in A\n");
    std::printf(" 9. Get element from A\n");
    std::printf("10. GetSubsequence of A\n");
    std::printf("11. Map A (x * 2)\n");
    std::printf("12. Where A (x > 0)\n");
    std::printf("13. Reduce A (sum, N elements)\n");
    std::printf("14. Concat A + B (make B first with option 2)\n");
    std::printf("15. Stream demo\n");
    std::printf("16. Ordinal algebra menu\n");
    std::printf(" 0. Exit\n");
}

void ConsoleUi::Run() {
    while (true) {
        PrintMenu();
        int choice = ReadMenuChoice();
        try {
            switch (choice) {
                case 0: return;
                case 1:  RunTests();        break;
                case 2:  MakeLazySequence(); break;
                case 3: {
                    delete seq_a_;
                    int seed[] = {1, 1};
                    seq_a_ = new LazySequence<int>(FibRule, seed, 2);
                    std::printf("A = infinite Fibonacci sequence\n");
                    break;
                }
                case 4: {
                    delete seq_a_;
                    int seed[] = {0};
                    seq_a_ = new LazySequence<int>(NatRule, seed, 1);
                    std::printf("A = infinite naturals 0,1,2,...\n");
                    break;
                }
                case 5:  ShowSequence();    break;
                case 6:  DoAppend();        break;
                case 7:  DoPrepend();       break;
                case 8:  DoInsertAt();      break;
                case 9:  DoGetElement();    break;
                case 10: DoGetSubsequence(); break;
                case 11: DoMap();           break;
                case 12: DoWhere();         break;
                case 13: DoReduce();        break;
                case 14: DoConcat();        break;
                case 15: StreamDemo();      break;
                case 16: OrdinalMenu();     break;
                default: std::printf("unknown choice\n");
            }
        } catch (const std::exception& ex) {
            std::printf("error: %s\n", ex.what());
        }
    }
}

void ConsoleUi::RunTests() { tests::RunAllTests(); }

void ConsoleUi::MakeLazySequence() {
    int vals[256]; int count = 0;
    ReadIntList("values (space-separated): ", vals, 256, &count);
    delete seq_a_;
    seq_a_ = new LazySequence<int>(vals, count);
    std::printf("A created, length %d\n", count);
}

void ConsoleUi::PrintLazySeq(const char* label, LazySequence<int>* seq, int show_count) const {
    if (!seq) { std::printf("%s = (null)\n", label); return; }
    std::printf("%s [length=%s, materialized=%zu] = [",
        label,
        seq->GetLength().ToString().c_str(),
        seq->GetMaterializedCount());
    bool inf = seq->GetLength().IsOmega();
    int n = inf ? show_count : (int)seq->GetLength().FiniteValue();
    for (int i = 0; i < n; ++i) {
        if (i > 0) std::printf(", ");
        try { std::printf("%d", seq->Get(i)); }
        catch (...) { std::printf("?"); break; }
    }
    if (inf) std::printf(", ...");
    std::printf("]\n");
}

void ConsoleUi::ShowSequence() {
    if (!seq_a_) { std::printf("A is not initialized\n"); return; }
    int n = 10;
    if (seq_a_->GetLength().IsOmega())
        n = ReadInt("how many elements to show: ");
    PrintLazySeq("A", seq_a_, n);
}

void ConsoleUi::DoAppend() {
    if (!seq_a_) { std::printf("A not initialized\n"); return; }
    int v = ReadInt("value: ");
    seq_a_->Append(v);
    std::printf("done\n");
}

void ConsoleUi::DoPrepend() {
    if (!seq_a_) { std::printf("A not initialized\n"); return; }
    int v = ReadInt("value: ");
    seq_a_->Prepend(v);
    std::printf("done\n");
}

void ConsoleUi::DoInsertAt() {
    if (!seq_a_) { std::printf("A not initialized\n"); return; }
    int v   = ReadInt("value: ");
    int idx = ReadInt("index: ");
    seq_a_->InsertAt(v, idx);
    std::printf("done\n");
}

void ConsoleUi::DoGetElement() {
    if (!seq_a_) { std::printf("A not initialized\n"); return; }
    int idx = ReadInt("index: ");
    int val = seq_a_->Get(idx);
    std::printf("A[%d] = %d\n", idx, val);
}

void ConsoleUi::DoGetSubsequence() {
    if (!seq_a_) { std::printf("A not initialized\n"); return; }
    int s = ReadInt("start index: ");
    int e = ReadInt("end index: ");
    auto* sub = seq_a_->GetSubsequence(s, e);
    PrintLazySeq("sub", sub, (int)sub->GetLength().FiniteValue());
    delete sub;
}

void ConsoleUi::DoMap() {
    if (!seq_a_) { std::printf("A not initialized\n"); return; }
    auto* r = seq_a_->Map<int>([](const int& x) { return x * 2; });
    std::printf("Map x*2: ");
    PrintLazySeq("result", r, 10);
    delete r;
}

void ConsoleUi::DoWhere() {
    if (!seq_a_) { std::printf("A not initialized\n"); return; }
    auto* r = seq_a_->Where([](const int& x) { return x > 0; });
    std::printf("Where x>0: ");
    PrintLazySeq("result", r, (int)r->GetLength().FiniteValue());
    delete r;
}

void ConsoleUi::DoReduce() {
    if (!seq_a_) { std::printf("A not initialized\n"); return; }
    int n = ReadInt("how many elements: ");
    int sum = seq_a_->Reduce<int>([](const int& acc, const int& x) { return acc + x; }, 0, (size_t)n);
    std::printf("sum of first %d elements = %d\n", n, sum);
}

void ConsoleUi::DoConcat() {
    if (!seq_a_) { std::printf("A not initialized\n"); return; }
    int vals[256]; int count = 0;
    ReadIntList("values for B (space-separated): ", vals, 256, &count);
    delete seq_b_;
    seq_b_ = new LazySequence<int>(vals, count);
    seq_a_->Concat(seq_b_);
    std::printf("A after concat: ");
    PrintLazySeq("A", seq_a_, 20);
}

void ConsoleUi::StreamDemo() {
    std::printf("\n-- Stream demo --\n");
    WriteOnlyStream<int> ws;
    ws.Open();
    for (int i = 1; i <= 10; ++i) ws.Write(i);
    ws.Close();

    ReadOnlyStream<int> rs(ws.Data());
    rs.Open();
    std::printf("stream: ");
    int sum = 0;
    while (!rs.IsEndOfStream()) {
        int v = rs.Read();
        std::printf("%d ", v);
        sum += v;
    }
    std::printf("\nsum = %d\n", sum);
    rs.Close();

    // seek demo
    rs.Seek(0);
    rs.Open();
    rs.Seek(4); std::printf("seek to pos 4, read: %d\n", rs.Read());
    rs.Close();

    // string stream
    ReadOnlyStream<int> ss("100 200 300", ParseInt);
    ss.Open();
    std::printf("from string: ");
    while (!ss.IsEndOfStream()) std::printf("%d ", ss.Read());
    std::printf("\n");
    ss.Close();

    // file stream demo
    {
        const char* fname = "/tmp/lab4_demo_stream.txt";
        {
            std::ofstream f(fname);
            for (int i = 1; i <= 20; ++i) f << i << " ";
        }
        ReadOnlyStream<int> fs(std::string(fname), true, ParseInt);
        fs.Open();
        std::printf("from file (1..20): ");
        int s = 0;
        while (!fs.IsEndOfStream()) { int v = fs.Read(); std::printf("%d ", v); s += v; }
        std::printf("\nsum = %d\n", s);
        fs.Close();
        std::remove(fname);
    }
}

void ConsoleUi::OrdinalMenu() {
    while (true) {
        std::printf("\n-- Ordinal algebra --\n");
        std::printf(" 1. Enter two ordinals and compute +, *, ^\n");
        std::printf(" 2. Show ordinal examples\n");
        std::printf(" 0. Back\n");
        int c = ReadMenuChoice();
        if (c == 0) return;
        if (c == 2) {
            auto w  = Ordinal::Omega();
            auto w2 = w.Pow(Ordinal::Finite(2));
            auto w3 = w.Pow(Ordinal::Finite(3));
            auto ww = w.Pow(w);
            std::printf("w           = %s\n", w.ToString().c_str());
            std::printf("w^2         = %s\n", w2.ToString().c_str());
            std::printf("w^3         = %s\n", w3.ToString().c_str());
            std::printf("w^w         = %s\n", ww.ToString().c_str());
            std::printf("w^2 + w + 1 = %s\n", (w2 + w + Ordinal::Finite(1)).ToString().c_str());
            std::printf("w * 3       = %s\n", (w * Ordinal::Finite(3)).ToString().c_str());
            std::printf("3 * w       = %s\n", (Ordinal::Finite(3) * w).ToString().c_str());
            std::printf("2^w         = %s\n", Ordinal::Finite(2).Pow(w).ToString().c_str());
            std::printf("w^(w^2)     = %s\n", w.Pow(w2).ToString().c_str());
            continue;
        }
        if (c == 1) {
            std::printf("ordinal format: 0..N | w | w+N | w*N | w^K | w^K+w^J+N\n");
            std::printf("first ordinal: ");
            std::string sa, sb;
            std::getline(std::cin, sa);
            std::printf("second ordinal: ");
            std::getline(std::cin, sb);
            try {
                Ordinal a = ParseOrdinal(sa);
                Ordinal b = ParseOrdinal(sb);
                std::printf("%s + %s = %s\n", a.ToString().c_str(), b.ToString().c_str(), (a + b).ToString().c_str());
                std::printf("%s * %s = %s\n", a.ToString().c_str(), b.ToString().c_str(), (a * b).ToString().c_str());
                std::printf("%s ^ %s = %s\n", a.ToString().c_str(), b.ToString().c_str(), a.Pow(b).ToString().c_str());
            } catch (const std::exception& e) {
                std::printf("parse error: %s\n", e.what());
            }
        }
    }
}

int ConsoleUi::ReadInt(const char* prompt) const {
    std::printf("%s", prompt);
    std::string line;
    if (!std::getline(std::cin, line)) throw InvalidArgument("input ended");
    std::stringstream ss(line);
    int v = 0;
    if (!(ss >> v)) throw InvalidArgument("not an integer");
    return v;
}

bool ConsoleUi::ReadIntList(const char* prompt, int* out, int max, int* count) {
    std::printf("%s", prompt);
    std::string line;
    if (!std::getline(std::cin, line)) return false;
    std::stringstream ss(line);
    int n = 0, x;
    while (ss >> x && n < max) out[n++] = x;
    *count = n;
    return true;
}

int ConsoleUi::ReadMenuChoice() const {
    std::string line;
    std::printf("> ");
    if (!std::getline(std::cin, line)) return 0;
    std::stringstream ss(line);
    int v = -1; ss >> v;
    return v;
}

}  // namespace lab4
