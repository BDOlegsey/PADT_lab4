#ifndef LAB4_STREAM_TPP
#define LAB4_STREAM_TPP

#include "stream.h"
#include <sstream>
#include <string>

namespace lab4 {

// ── ReadOnlyStream ────────────────────────────────────────────────────────────

template <class T>
ReadOnlyStream<T>::ReadOnlyStream(const std::vector<T>& data)
    : data_(data), cursor_(0), open_(false) {}

template <class T>
ReadOnlyStream<T>::ReadOnlyStream(const T* data, size_t count)
    : cursor_(0), open_(false) {
    data_.assign(data, data + count);
}

template <class T>
ReadOnlyStream<T>::ReadOnlyStream(const std::string& str,
                                   std::function<T(const std::string&)> deser)
    : cursor_(0), open_(false) {
    std::istringstream ss(str);
    std::string token;
    while (ss >> token) data_.push_back(deser(token));
}

template <class T>
void ReadOnlyStream<T>::Open()  { open_ = true; cursor_ = 0; }

template <class T>
void ReadOnlyStream<T>::Close() { open_ = false; }

template <class T>
bool ReadOnlyStream<T>::IsEndOfStream() const { return cursor_ >= data_.size(); }

template <class T>
T ReadOnlyStream<T>::Read() {
    if (IsEndOfStream()) throw EndOfStream();
    return data_[cursor_++];
}

template <class T>
Option<T> ReadOnlyStream<T>::TryRead() {
    if (IsEndOfStream()) return Option<T>::None();
    return Option<T>::Some(data_[cursor_++]);
}

template <class T>
size_t ReadOnlyStream<T>::GetPosition() const { return cursor_; }

template <class T>
bool ReadOnlyStream<T>::IsCanSeek() const { return true; }

template <class T>
size_t ReadOnlyStream<T>::Seek(size_t index) {
    if (index > data_.size()) index = data_.size();
    cursor_ = index;
    return cursor_;
}

template <class T>
bool ReadOnlyStream<T>::IsCanGoBack() const { return true; }

// ── WriteOnlyStream ───────────────────────────────────────────────────────────

template <class T>
WriteOnlyStream<T>::WriteOnlyStream() : sink_(nullptr), position_(0), open_(false) {}

template <class T>
WriteOnlyStream<T>::WriteOnlyStream(std::vector<T>* sink)
    : sink_(sink), position_(0), open_(false) {}

template <class T>
void WriteOnlyStream<T>::Open()  { open_ = true; }

template <class T>
void WriteOnlyStream<T>::Close() { open_ = false; }

template <class T>
size_t WriteOnlyStream<T>::Write(const T& item) {
    if (sink_) sink_->push_back(item);
    else internal_.push_back(item);
    return ++position_;
}

template <class T>
size_t WriteOnlyStream<T>::GetPosition() const { return position_; }

template <class T>
const std::vector<T>& WriteOnlyStream<T>::Data() const {
    return sink_ ? *sink_ : internal_;
}

}  // namespace lab4
#endif
