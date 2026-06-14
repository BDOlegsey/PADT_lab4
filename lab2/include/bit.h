#ifndef LAB2_BIT_H
#define LAB2_BIT_H

namespace lab2 {

class Bit {
public:
    Bit() : value_(false) {}
    Bit(bool value) : value_(value) {}

    bool Value() const { return value_; }
    char Char() const { return value_ ? '1' : '0'; }

    Bit operator&(const Bit& other) const { return Bit(value_ && other.value_); }
    Bit operator|(const Bit& other) const { return Bit(value_ || other.value_); }
    Bit operator^(const Bit& other) const { return Bit(value_ != other.value_); }
    Bit operator~() const { return Bit(!value_); }

    bool operator==(const Bit& other) const { return value_ == other.value_; }
    bool operator!=(const Bit& other) const { return value_ != other.value_; }

private:
    bool value_;
};

inline char ToChar(const Bit& bit) { return bit.Value() ? '1' : '0'; }

}  // namespace lab2

#endif
