#include "../include/errors.h"
#include "../include/stream.h"
#include "../lab2/include/dynamic_array.h"
#include "test_framework.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>

namespace lab4 {
namespace tests {

static int ParseInt(const std::string& s) {
    return std::stoi(s);
}

static void ArrayPushBack(lab2::DynamicArray<int>& array, int value) {
    int size = array.GetSize();
    array.Resize(size + 1);
    array.Set(size, value);
}

void TestStream() {
    std::printf("\n-- Stream --\n");

    {
        int arr[] = {1, 2, 3, 4, 5};
        ReadOnlyStream<int> rs(arr, 5);
        rs.Open();
        T_ASSERT("ros not end", !rs.IsEndOfStream());
        T_ASSERT_EQ("ros read 1", rs.Read(), 1);
        T_ASSERT_EQ("ros pos after 1", rs.GetPosition(), 1u);
        T_ASSERT_EQ("ros read 2", rs.Read(), 2);
        T_ASSERT("ros can seek", rs.IsCanSeek());
        rs.Seek(4);
        T_ASSERT_EQ("ros seek read 5", rs.Read(), 5);
        T_ASSERT("ros end", rs.IsEndOfStream());
        T_ASSERT_THROWS("ros eos throws", EndOfStream, rs.Read());
        rs.Close();
    }

    {
        ReadOnlyStream<int> rs("10 20 30", ParseInt);
        rs.Open();
        T_ASSERT_EQ("str read 10", rs.Read(), 10);
        T_ASSERT_EQ("str read 20", rs.Read(), 20);
        T_ASSERT_EQ("str read 30", rs.Read(), 30);
        T_ASSERT("str eos", rs.IsEndOfStream());
        rs.Close();
    }

    {
        int arr[] = {7};
        ReadOnlyStream<int> rs(arr, 1);
        rs.Open();
        auto opt = rs.TryRead();
        T_ASSERT("try read some", opt.HasValue());
        T_ASSERT_EQ("try value", opt.Value(), 7);
        auto opt2 = rs.TryRead();
        T_ASSERT("try read none", opt2.IsNone());
        rs.Close();
    }

    {
        int arr[] = {1, 2, 3};
        ReadOnlyStream<int> rs(arr, 3);
        rs.Open();
        rs.Read();
        rs.Read();
        T_ASSERT("can go back", rs.IsCanGoBack());
        rs.Seek(0);
        T_ASSERT_EQ("after goback read 1", rs.Read(), 1);
        rs.Close();
    }

    {
        WriteOnlyStream<int> ws;
        ws.Open();
        T_ASSERT_EQ("write 1 pos", ws.Write(10), 1u);
        T_ASSERT_EQ("write 2 pos", ws.Write(20), 2u);
        T_ASSERT_EQ("write 3 pos", ws.Write(30), 3u);
        T_ASSERT_EQ("ws position", ws.GetPosition(), 3u);
        T_ASSERT_EQ("ws data[0]", ws.Data().Get(0), 10);
        T_ASSERT_EQ("ws data[2]", ws.Data().Get(2), 30);
        ws.Close();
    }

    {
        lab2::DynamicArray<int> sink;
        WriteOnlyStream<int> ws(&sink);
        ws.Open();
        ws.Write(42);
        ws.Write(43);
        T_ASSERT_EQ("sink size", static_cast<size_t>(sink.GetSize()), 2u);
        T_ASSERT_EQ("sink[0]", sink.Get(0), 42);
        ws.Close();
    }

    {
        WriteOnlyStream<int> ws;
        ws.Open();
        for (int i = 0; i < 5; ++i) {
            ws.Write(i * 10);
        }
        ws.Close();
        ReadOnlyStream<int> rs(ws.Data());
        rs.Open();
        int sum = 0;
        while (!rs.IsEndOfStream()) {
            sum += rs.Read();
        }
        T_ASSERT_EQ("round-trip sum", sum, 100);
        rs.Close();
    }

    {
        const int kN = 1000000;
        lab2::DynamicArray<int> big;
        for (int i = 0; i < kN; ++i) {
            ArrayPushBack(big, i);
        }
        ReadOnlyStream<int> rs(big);
        rs.Open();
        long long sum = 0;
        while (!rs.IsEndOfStream()) {
            sum += rs.Read();
        }
        long long expected = static_cast<long long>(kN) * (kN - 1) / 2;
        T_ASSERT_EQ("1M stream sum", sum, expected);
        rs.Close();
    }

    {
        const int kN = 100000;
        char fname_buf[L_tmpnam];
        if (!std::tmpnam(fname_buf)) {
            throw InvalidArgument("Unable to create temporary file name");
        }
        std::string fname(fname_buf);
        if (!fname.empty() && (fname[0] == '/' || fname[0] == '\\')) {
            fname = std::string(".") + fname;
        }
        {
            std::ofstream file(fname);
            for (int i = 0; i < kN; ++i) {
                file << i << " ";
            }
        }
        ReadOnlyStream<int> rs(fname, true, ParseInt);
        rs.Open();
        long long sum = 0;
        while (!rs.IsEndOfStream()) {
            sum += rs.Read();
        }
        long long expected = static_cast<long long>(kN) * (kN - 1) / 2;
        T_ASSERT_EQ("file stream 100k", sum, expected);
        rs.Close();
        std::remove(fname.c_str());
    }
}

}  // namespace tests
}  // namespace lab4
