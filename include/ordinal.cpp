#include "ordinal.h"
#include "errors.h"
#include <algorithm>
#include <sstream>

namespace lab4 {

Ordinal::Ordinal() {} 

Ordinal::Ordinal(uint64_t n) {
    if (n > 0) {
        Term t;
        t.exponent = new Ordinal(); 
        t.coef = n;
        terms_.push_back(t);
    }
}

Ordinal::Ordinal(const Ordinal& other) {
    for (const auto& t : other.terms_) {
        Term nt;
        nt.exponent = new Ordinal(*t.exponent);
        nt.coef = t.coef;
        terms_.push_back(nt);
    }
}

Ordinal& Ordinal::operator=(const Ordinal& other) {
    if (this == &other) return *this;
    for (auto& t : terms_) delete t.exponent;
    terms_.clear();
    for (const auto& t : other.terms_) {
        Term nt;
        nt.exponent = new Ordinal(*t.exponent);
        nt.coef = t.coef;
        terms_.push_back(nt);
    }
    return *this;
}

Ordinal::~Ordinal() {
    for (auto& t : terms_) delete t.exponent;
}


Ordinal Ordinal::Zero() { return Ordinal(); }

Ordinal Ordinal::Finite(uint64_t n) { return Ordinal(n); }

Ordinal Ordinal::Omega() {
    Ordinal r;
    Term t;
    t.exponent = new Ordinal(Finite(1));
    t.coef = 1;
    r.terms_.push_back(t);
    return r;
}

Ordinal Ordinal::FromTerms(std::vector<Term>&& terms) {
    Ordinal r;
    r.terms_ = std::move(terms);
    return r;
}


bool Ordinal::IsZero() const { return terms_.empty(); }

bool Ordinal::IsFinite() const {
    if (terms_.empty()) return true;
    if (terms_.size() > 1) return false;
    return terms_[0].exponent->IsZero();
}

bool Ordinal::IsTransfinite() const { return !IsFinite(); }

uint64_t Ordinal::FiniteValue() const {
    if (terms_.empty()) return 0;
    if (!IsFinite()) throw InvalidArgument("Ordinal: not finite");
    return terms_[0].coef;
}


bool Ordinal::operator==(const Ordinal& o) const {
    if (terms_.size() != o.terms_.size()) return false;
    for (size_t i = 0; i < terms_.size(); ++i) {
        if (terms_[i].coef != o.terms_[i].coef) return false;
        if (!(*terms_[i].exponent == *o.terms_[i].exponent)) return false;
    }
    return true;
}

bool Ordinal::operator!=(const Ordinal& o) const { return !(*this == o); }

bool Ordinal::operator<(const Ordinal& o) const {
    size_t mn = std::min(terms_.size(), o.terms_.size());
    for (size_t i = 0; i < mn; ++i) {
        const Ordinal& ea = *terms_[i].exponent;
        const Ordinal& eb = *o.terms_[i].exponent;
        if (ea != eb) return ea < eb;
        if (terms_[i].coef != o.terms_[i].coef) return terms_[i].coef < o.terms_[i].coef;
    }
    return terms_.size() < o.terms_.size();
}

bool Ordinal::operator<=(const Ordinal& o) const { return !(o < *this); }
bool Ordinal::operator>(const Ordinal& o) const  { return o < *this; }


Ordinal Ordinal::operator+(const Ordinal& other) const {
    if (other.IsZero()) return *this;
    if (IsZero())       return other;

    const Ordinal& head_other_exp = *other.terms_[0].exponent;
    std::vector<Term> result;

    for (const auto& t : terms_) {
        if (*t.exponent > head_other_exp) {
            Term nt; nt.exponent = new Ordinal(*t.exponent); nt.coef = t.coef;
            result.push_back(nt);
        } else {
            break;
        }
    }

    bool merged = false;
    for (const auto& t : terms_) {
        if (*t.exponent == head_other_exp) {
            Term nt; nt.exponent = new Ordinal(head_other_exp); nt.coef = t.coef + other.terms_[0].coef;
            result.push_back(nt);
            merged = true;
            break;
        }
    }
    if (!merged) {
        Term nt; nt.exponent = new Ordinal(head_other_exp); nt.coef = other.terms_[0].coef;
        result.push_back(nt);
    }

    for (size_t i = 1; i < other.terms_.size(); ++i) {
        Term nt; nt.exponent = new Ordinal(*other.terms_[i].exponent); nt.coef = other.terms_[i].coef;
        result.push_back(nt);
    }

    return FromTerms(std::move(result));
}


Ordinal Ordinal::operator*(const Ordinal& other) const {
    if (IsZero() || other.IsZero()) return Zero();
    if (other.IsFinite()) {
        // alpha * n: multiply leading coef by n, keep rest
        uint64_t n = other.FiniteValue();
        if (n == 0) return Zero();
        std::vector<Term> result;
        for (size_t i = 0; i < terms_.size(); ++i) {
            Term nt;
            nt.exponent = new Ordinal(*terms_[i].exponent);
            nt.coef = (i == 0) ? terms_[0].coef * n : terms_[i].coef;
            result.push_back(nt);
        }
        return FromTerms(std::move(result));
    }

    Ordinal result = Zero();
    for (const auto& ot : other.terms_) {
        const Ordinal& ot_exp = *ot.exponent;
        uint64_t ot_c = ot.coef;
        Ordinal piece;
        if (ot_exp.IsZero()) {
            piece = *this * Finite(ot_c);
        } else {
            std::vector<Term> terms;
            if (IsFinite()) {
                Term t; t.exponent = new Ordinal(ot_exp); t.coef = ot_c;
                terms.push_back(t);
            } else {
                Ordinal new_exp = *terms_[0].exponent + ot_exp;
                Term t; t.exponent = new Ordinal(new_exp); t.coef = ot_c;
                terms.push_back(t);
            }
            piece = FromTerms(std::move(terms));
        }
        result = result + piece;
    }
    return result;
}


Ordinal Ordinal::IntPow(uint64_t n) const {
    if (n == 0) return Finite(1);
    Ordinal result = Finite(1);
    Ordinal base = *this;
    while (n > 0) {
        if (n & 1) result = result * base;
        base = base * base;
        n >>= 1;
    }
    return result;
}

Ordinal Ordinal::ShiftExpDown(const Ordinal& e) {
    if (e.IsZero()) return Zero(); 
    if (e.IsFinite()) {
        uint64_t k = e.FiniteValue();
        if (k == 0) return Zero();
        return Finite(k - 1);
    }
    std::vector<Ordinal::Term> result;
    for (const auto& t : e.Terms()) {
        result.push_back({new Ordinal(*t.exponent), t.coef});
    }

    if (!result.empty() && result.back().exponent->IsZero()) {
        if (result.back().coef > 1) {
            result.back().coef -= 1;
        } else {
            delete result.back().exponent;
            result.pop_back();
        }
        return Ordinal::FromTerms(std::move(result));
    }

    return e;
}

Ordinal Ordinal::Pow(const Ordinal& exp) const {
    if (exp.IsZero()) return Finite(1);
    if (IsZero()) return Zero();
    if (*this == Finite(1)) return Finite(1);

    if (exp.IsFinite()) {
        uint64_t n = exp.FiniteValue();
        if (IsTransfinite()) {
            return IntPow(n);
        }
        // finite^finite
        return IntPow(n);
    }

    uint64_t r_val = 0;
    std::vector<Term> alpha_terms;
    for (const auto& t : exp.Terms()) {
        if (t.exponent->IsZero()) {
            r_val = t.coef;
        } else {
            Ordinal new_exp = ShiftExpDown(*t.exponent);
            Term nt; nt.exponent = new Ordinal(new_exp); nt.coef = t.coef;
            alpha_terms.push_back(nt);
        }
    }
    Ordinal alpha = FromTerms(std::move(alpha_terms));

    Ordinal fin_pow = IntPow(r_val);

    if (alpha.IsZero()) {
        return fin_pow;
    }

    Ordinal base_pow;
    if (IsFinite()) {
        {
            std::vector<Term> ts;
            Term t;
            t.exponent = new Ordinal(alpha);
            t.coef = 1;
            ts.push_back(t);
            base_pow = Ordinal::FromTerms(std::move(ts));
        }
    } else {
        Ordinal a = *terms_[0].exponent;
        Ordinal a_times_alpha = a * Ordinal::Omega() * alpha;
        std::vector<Term> ts;
        Term t; t.exponent = new Ordinal(a_times_alpha); t.coef = 1;
        ts.push_back(t);
        base_pow = FromTerms(std::move(ts));
    }

    return base_pow * fin_pow;
}

std::string Ordinal::ToString() const {
    if (IsZero()) return "0";
    std::string s;
    for (size_t i = 0; i < terms_.size(); ++i) {
        if (i > 0) s += " + ";
        const Ordinal& e = *terms_[i].exponent;
        uint64_t c = terms_[i].coef;
        if (e.IsZero()) {
            s += std::to_string(c);
        } else if (e == Finite(1)) {
            s += (c == 1) ? "w" : ("w*" + std::to_string(c));
        } else {
            std::string es = e.ToString();
            bool need_paren = (e.Terms().size() > 1);
            if (need_paren) es = "(" + es + ")";
            s += "w^" + es;
            if (c > 1) s += "*" + std::to_string(c);
        }
    }
    return s;
}

}  // namespace lab4
