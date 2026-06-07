#include "../include/errors.h"
#include "test_framework.h"
#include "../include/stream.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>

namespace lab4 {
namespace tests {

static int ParseInt(const std::string& s) { return std::stoi(s); }

void TestStream() {
    std::printf("\n-- Stream --\n");

    // ReadOnlyStream from array
    {
        int arr[] = {1, 2, 3, 4, 5};
        ReadOnlyStream<int> rs(arr, 5);
        rs.Open();
        T_ASSERT("ros not end",           !rs.IsEndOfStream());
        T_ASSERT_EQ("ros read 1",         rs.Read(), 1);
        T_ASSERT_EQ("ros pos after 1",    rs.GetPosition(), 1u);
        T_ASSERT_EQ("ros read 2",         rs.Read(), 2);
        T_ASSERT("ros can seek",          rs.IsCanSeek());
        rs.Seek(4);
        T_ASSERT_EQ("ros seek read 5",    rs.Read(), 5);
        T_ASSERT("ros end",               rs.IsEndOfStream());
        T_ASSERT_THROWS("ros eos throws", EndOfStream, rs.Read());
        rs.Close();
    }

    // ReadOnlyStream from string with named deserializer
    {
        ReadOnlyStream<int> rs("10 20 30", ParseInt);
        rs.Open();
        T_ASSERT_EQ("str read 10", rs.Read(), 10);
        T_ASSERT_EQ("str read 20", rs.Read(), 20);
        T_ASSERT_EQ("str read 30", rs.Read(), 30);
        T_ASSERT("str eos",        rs.IsEndOfStream());
        rs.Close();
    }

    // TryRead
    {
        int arr[] = {7};
        ReadOnlyStream<int> rs(arr, 1);
        rs.Open();
        auto opt = rs.TryRead();
        T_ASSERT("try read some",  opt.HasValue());
        T_ASSERT_EQ("try value",   opt.Value(), 7);
        auto opt2 = rs.TryRead();
        T_ASSERT("try read none",  opt2.IsNone());
        rs.Close();
    }

    // GoBack (seek back)
    {
        int arr[] = {1, 2, 3};
        ReadOnlyStream<int> rs(arr, 3);
        rs.Open();
        rs.Read(); rs.Read();
        T_ASSERT("can go back", rs.IsCanGoBack());
        rs.Seek(0);
        T_ASSERT_EQ("after goback read 1", rs.Read(), 1);
        rs.Close();
    }

    // WriteOnlyStream
    {
        WriteOnlyStream<int> ws;
        ws.Open();
        T_ASSERT_EQ("write 1 pos", ws.Write(10), 1u);
        T_ASSERT_EQ("write 2 pos", ws.Write(20), 2u);
        T_ASSERT_EQ("write 3 pos", ws.Write(30), 3u);
        T_ASSERT_EQ("ws position", ws.GetPosition(), 3u);
        T_ASSERT_EQ("ws data[0]",  ws.Data()[0], 10);
        T_ASSERT_EQ("ws data[2]",  ws.Data()[2], 30);
        ws.Close();
    }

    // WriteOnly with sink
    {
        std::vector<int> sink;
        WriteOnlyStream<int> ws(&sink);
        ws.Open();
        ws.Write(42); ws.Write(43);
        T_ASSERT_EQ("sink size",  sink.size(), 2u);
        T_ASSERT_EQ("sink[0]",    sink[0], 42);
        ws.Close();
    }

    // Stream round-trip: write then read
    {
        WriteOnlyStream<int> ws;
        ws.Open();
        for (int i = 0; i < 5; ++i) ws.Write(i * 10);
        ws.Close();
        ReadOnlyStream<int> rs(ws.Data());
        rs.Open();
        int sum = 0;
        while (!rs.IsEndOfStream()) sum += rs.Read();
        T_ASSERT_EQ("round-trip sum", sum, 100);
        rs.Close();
    }

    // large in-memory stream: 1 million elements
    {
        const int kN = 1000000;
        std::vector<int> big;
        big.reserve(kN);
        for (int i = 0; i < kN; ++i) big.push_back(i);
        ReadOnlyStream<int> rs(big);
        rs.Open();
        long long sum = 0;
        while (!rs.IsEndOfStream()) sum += rs.Read();
        long long expected = (long long)kN * (kN - 1) / 2;
        T_ASSERT_EQ("1M stream sum", sum, expected);
        rs.Close();
    }

    // file-based stream: write 100k ints to temp file, read back
    {
        const int kN = 100000;
        const char* fname = "/tmp/lab4_stream_test.txt";
        {
            std::ofstream f(fname);
            for (int i = 0; i < kN; ++i) f << i << " ";
        }
        ReadOnlyStream<int> rs(std::string(fname), true, ParseInt);
        rs.Open();
        long long sum = 0;
        while (!rs.IsEndOfStream()) sum += rs.Read();
        long long expected = (long long)kN * (kN - 1) / 2;
        T_ASSERT_EQ("file stream 100k", sum, expected);
        rs.Close();
        std::remove(fname);
    }
}

}  // namespace tests
}  // namespace lab4
