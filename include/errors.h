#ifndef LAB4_ERRORS_H
#define LAB4_ERRORS_H

#include <stdexcept>
#include <string>

namespace lab4 {

class IndexOutOfRange : public std::out_of_range {
public:
    explicit IndexOutOfRange(const std::string& msg)
        : std::out_of_range("IndexOutOfRange: " + msg) {}
};

class EmptyContainer : public std::logic_error {
public:
    explicit EmptyContainer(const std::string& msg)
        : std::logic_error("EmptyContainer: " + msg) {}
};

class InvalidArgument : public std::invalid_argument {
public:
    explicit InvalidArgument(const std::string& msg)
        : std::invalid_argument("InvalidArgument: " + msg) {}
};

class NoneValueAccess : public std::logic_error {
public:
    NoneValueAccess() : std::logic_error("NoneValueAccess: option holds no value") {}
};

class EndOfStream : public std::runtime_error {
public:
    explicit EndOfStream(const std::string& msg = "end of stream reached")
        : std::runtime_error("EndOfStream: " + msg) {}
};

}  // namespace lab4

#endif
