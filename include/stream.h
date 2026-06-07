#ifndef LAB4_STREAM_H
#define LAB4_STREAM_H

#include <cstddef>
#include <functional>
#include <memory>
#include <vector>
#include "errors.h"
#include "option.h"

namespace lab4 {

template <class T> class LazySequence;

template <class T>
class ReadOnlyStream {
public:
    explicit ReadOnlyStream(const std::vector<T>& data);
    ReadOnlyStream(const T* data, size_t count);
    ReadOnlyStream(const std::string& str, std::function<T(const std::string&)> deser);
    ReadOnlyStream(const std::string& filename, bool is_file,
                   std::function<T(const std::string&)> deser);

    void Open();
    void Close();

    bool IsEndOfStream() const;
    T Read();
    Option<T> TryRead();

    size_t GetPosition() const;
    bool IsCanSeek() const;
    size_t Seek(size_t index);
    bool IsCanGoBack() const;

private:
    std::vector<T> data_;
    size_t cursor_;
    bool open_;
};

template <class T>
class WriteOnlyStream {
public:
    WriteOnlyStream();
    explicit WriteOnlyStream(std::vector<T>* sink);

    void Open();
    void Close();

    size_t Write(const T& item);
    size_t GetPosition() const;

    // retrieve all written data
    const std::vector<T>& Data() const;

private:
    std::vector<T> internal_;
    std::vector<T>* sink_;
    size_t position_;
    bool open_;
};

}  // namespace lab4

#include "stream.tpp"
#endif
