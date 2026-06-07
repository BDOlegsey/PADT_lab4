#include "cardinal.h"
#include "../include/errors.h"
#include <stdexcept>

namespace lab4 {

Cardinal::Cardinal(bool is_omega, size_t value) : is_omega_(is_omega), value_(value) {}

Cardinal Cardinal::Finite(size_t n) { return Cardinal(false, n); }
Cardinal Cardinal::Omega()          { return Cardinal(true,  0); }

bool   Cardinal::IsFinite() const { return !is_omega_; }
bool   Cardinal::IsOmega()  const { return  is_omega_; }

size_t Cardinal::FiniteValue() const {
    if (is_omega_) throw InvalidArgument("Cardinal: omega has no finite value");
    return value_;
}

Cardinal Cardinal::operator+(const Cardinal& other) const {
    if (is_omega_ || other.is_omega_) return Omega();
    return Finite(value_ + other.value_);
}

Cardinal Cardinal::operator+(size_t n) const {
    if (is_omega_) return Omega();
    return Finite(value_ + n);
}

bool Cardinal::operator==(const Cardinal& o) const {
    if (is_omega_ && o.is_omega_) return true;
    if (!is_omega_ && !o.is_omega_) return value_ == o.value_;
    return false;
}

bool Cardinal::operator!=(const Cardinal& o) const { return !(*this == o); }

bool Cardinal::operator<(const Cardinal& o) const {
    if (is_omega_) return false;
    if (o.is_omega_) return true;
    return value_ < o.value_;
}

bool Cardinal::operator<=(const Cardinal& o) const { return !(o < *this); }
bool Cardinal::operator>(const Cardinal& o) const  { return o < *this; }

std::string Cardinal::ToString() const {
    if (is_omega_) return "omega";
    return std::to_string(value_);
}

}  // namespace lab4
