#ifndef LAB4_STREAM_TPP
#define LAB4_STREAM_TPP

#include "stream.h"

#include <fstream>
#include <sstream>
#include <string>

namespace lab4 {

namespace {

template <class T>
void StreamArrayPushBack(lab2::DynamicArray<T>& array, const T& value) {
    int size = array.GetSize();
    array.Resize(size + 1);
    array.Set(size, value);
}

}  // namespace

template <class T>
ReadOnlyStream<T>::ReadOnlyStream(const lab2::DynamicArray<T>& data)
    : data_(data),
      cursor_(0),
      open_(false) {}

template <class T>
ReadOnlyStream<T>::ReadOnlyStream(const T* data, size_t count)
    : data_(data, static_cast<int>(count)),
      cursor_(0),
      open_(false) {}

template <class T>
ReadOnlyStream<T>::ReadOnlyStream(const std::string& str,
                                  std::function<T(const std::string&)> deser)
    : cursor_(0),
      open_(false) {
    std::istringstream input(str);
    std::string token;
    while (input >> token) {
        StreamArrayPushBack(data_, deser(token));
    }
}

template <class T>
ReadOnlyStream<T>::ReadOnlyStream(const std::string& filename, bool /*is_file*/,
                                  std::function<T(const std::string&)> deser)
    : cursor_(0),
      open_(false) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw InvalidArgument("ReadOnlyStream: cannot open file: " + filename);
    }

    std::string token;
    while (file >> token) {
        StreamArrayPushBack(data_, deser(token));
    }
}

template <class T>
void ReadOnlyStream<T>::Open() {
    open_ = true;
    cursor_ = 0;
}

template <class T>
void ReadOnlyStream<T>::Close() {
    open_ = false;
}

template <class T>
bool ReadOnlyStream<T>::IsEndOfStream() const {
    return cursor_ >= static_cast<size_t>(data_.GetSize());
}

template <class T>
T ReadOnlyStream<T>::Read() {
    if (IsEndOfStream()) {
        throw EndOfStream();
    }
    return data_.Get(static_cast<int>(cursor_++));
}

template <class T>
Option<T> ReadOnlyStream<T>::TryRead() {
    if (IsEndOfStream()) {
        return Option<T>::None();
    }
    return Option<T>::Some(data_.Get(static_cast<int>(cursor_++)));
}

template <class T>
size_t ReadOnlyStream<T>::GetPosition() const {
    return cursor_;
}

template <class T>
bool ReadOnlyStream<T>::IsCanSeek() const {
    return true;
}

template <class T>
size_t ReadOnlyStream<T>::Seek(size_t index) {
    if (index > static_cast<size_t>(data_.GetSize())) {
        index = static_cast<size_t>(data_.GetSize());
    }
    cursor_ = index;
    return cursor_;
}

template <class T>
bool ReadOnlyStream<T>::IsCanGoBack() const {
    return true;
}

template <class T>
WriteOnlyStream<T>::WriteOnlyStream()
    : sink_(nullptr),
      position_(0),
      open_(false) {}

template <class T>
WriteOnlyStream<T>::WriteOnlyStream(lab2::DynamicArray<T>* sink)
    : sink_(sink),
      position_(0),
      open_(false) {}

template <class T>
void WriteOnlyStream<T>::Open() {
    open_ = true;
}

template <class T>
void WriteOnlyStream<T>::Close() {
    open_ = false;
}

template <class T>
size_t WriteOnlyStream<T>::Write(const T& item) {
    if (sink_) {
        StreamArrayPushBack(*sink_, item);
    } else {
        StreamArrayPushBack(internal_, item);
    }
    return ++position_;
}

template <class T>
size_t WriteOnlyStream<T>::GetPosition() const {
    return position_;
}

template <class T>
const lab2::DynamicArray<T>& WriteOnlyStream<T>::Data() const {
    return sink_ ? *sink_ : internal_;
}

}  // namespace lab4

#endif
