#ifndef LAB2_ERRORS_H
#define LAB2_ERRORS_H

#include <stdexcept>
#include <string>

namespace lab2 {

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

class SizeMismatch : public std::logic_error {
public:
    explicit SizeMismatch(const std::string& msg)
        : std::logic_error("SizeMismatch: " + msg) {}
};

class NoneValueAccess : public std::logic_error {
public:
    NoneValueAccess() : std::logic_error("NoneValueAccess: option holds no value") {}
};

}  // namespace lab2

#endif
