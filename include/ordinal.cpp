#include "ordinal.h"
#include "errors.h"
#include <algorithm>
#include <sstream>

namespace {

void AppendTerm(lab2::DynamicArray<lab4::Ordinal::Term>& array,
                const lab4::Ordinal::Term& term) {
    int index = array.GetSize();
    array.Resize(index + 1);
    array.Set(index, term);
}

}  // namespace

namespace lab4 {

Ordinal::Ordinal() {}

Ordinal::Ordinal(uint64_t n) {
    if (n > 0) {
        Term t;
        t.exponent = new Ordinal();
        t.coef = n;
        AppendTerm(terms_, t);
    }
}

Ordinal::Ordinal(const Ordinal& other) {
    int count = other.terms_.GetSize();
    for (int i = 0; i < count; ++i) {
        const Term& t = other.terms_[i];
        Term nt;
        nt.exponent = new Ordinal(*t.exponent);
        nt.coef = t.coef;
        AppendTerm(terms_, nt);
    }
}

Ordinal& Ordinal::operator=(const Ordinal& other) {
    if (this == &other) return *this;
    int current_size = terms_.GetSize();
    for (int i = 0; i < current_size; ++i) delete terms_[i].exponent;
    terms_.Resize(0);
    int count = other.terms_.GetSize();
    for (int i = 0; i < count; ++i) {
        const Term& t = other.terms_[i];
        Term nt;
        nt.exponent = new Ordinal(*t.exponent);
        nt.coef = t.coef;
        AppendTerm(terms_, nt);
    }
    return *this;
}

Ordinal::~Ordinal() {
    int current_size = terms_.GetSize();
    for (int i = 0; i < current_size; ++i) delete terms_[i].exponent;
}


Ordinal Ordinal::Zero() { return Ordinal(); }

Ordinal Ordinal::Finite(uint64_t n) { return Ordinal(n); }

Ordinal Ordinal::Omega() {
    Ordinal r;
    Term t;
    t.exponent = new Ordinal(Finite(1));
    t.coef = 1;
    AppendTerm(r.terms_, t);
    return r;
}

Ordinal Ordinal::FromTerms(const lab2::DynamicArray<Term>& terms) {
    Ordinal r;
    r.terms_ = terms;
    return r;
}


bool Ordinal::IsZero() const { return terms_.GetSize() == 0; }

bool Ordinal::IsFinite() const {
    if (terms_.GetSize() == 0) return true;
    if (terms_.GetSize() > 1) return false;
    return terms_[0].exponent->IsZero();
}

bool Ordinal::IsTransfinite() const { return !IsFinite(); }

uint64_t Ordinal::FiniteValue() const {
    if (terms_.GetSize() == 0) return 0;
    if (!IsFinite()) throw InvalidArgument("Ordinal: not finite");
    return terms_[0].coef;
}


bool Ordinal::operator==(const Ordinal& o) const {
    if (terms_.GetSize() != o.terms_.GetSize()) return false;
    int count = terms_.GetSize();
    for (int i = 0; i < count; ++i) {
        if (terms_[i].coef != o.terms_[i].coef) return false;
        if (!(*terms_[i].exponent == *o.terms_[i].exponent)) return false;
    }
    return true;
}

bool Ordinal::operator!=(const Ordinal& o) const { return !(*this == o); }

bool Ordinal::operator<(const Ordinal& o) const {
    int lhs_size = terms_.GetSize();
    int rhs_size = o.terms_.GetSize();
    int mn = std::min(lhs_size, rhs_size);
    for (int i = 0; i < mn; ++i) {
        const Ordinal& ea = *terms_[i].exponent;
        const Ordinal& eb = *o.terms_[i].exponent;
        if (ea != eb) return ea < eb;
        if (terms_[i].coef != o.terms_[i].coef) return terms_[i].coef < o.terms_[i].coef;
    }
    return lhs_size < rhs_size;
}

bool Ordinal::operator<=(const Ordinal& o) const { return !(o < *this); }
bool Ordinal::operator>(const Ordinal& o) const  { return o < *this; }


Ordinal Ordinal::operator+(const Ordinal& other) const {
    if (other.IsZero()) return *this;
    if (IsZero())       return other;

    const Ordinal& head_other_exp = *other.terms_[0].exponent;
    lab2::DynamicArray<Term> result;

    int lhs_count = terms_.GetSize();
    for (int i = 0; i < lhs_count; ++i) {
        const Term& t = terms_[i];
        if (*t.exponent > head_other_exp) {
            Term nt; nt.exponent = new Ordinal(*t.exponent); nt.coef = t.coef;
            AppendTerm(result, nt);
        } else {
            break;
        }
    }

    bool merged = false;
    for (int i = 0; i < lhs_count; ++i) {
        const Term& t = terms_[i];
        if (*t.exponent == head_other_exp) {
            Term nt; nt.exponent = new Ordinal(head_other_exp); nt.coef = t.coef + other.terms_[0].coef;
            AppendTerm(result, nt);
            merged = true;
            break;
        }
    }
    if (!merged) {
        Term nt; nt.exponent = new Ordinal(head_other_exp); nt.coef = other.terms_[0].coef;
        AppendTerm(result, nt);
    }

    int other_count = other.terms_.GetSize();
    for (int i = 1; i < other_count; ++i) {
        Term nt; nt.exponent = new Ordinal(*other.terms_[i].exponent); nt.coef = other.terms_[i].coef;
        AppendTerm(result, nt);
    }

    return FromTerms(result);
}


Ordinal Ordinal::operator*(const Ordinal& other) const {
    if (IsZero() || other.IsZero()) return Zero();
    if (other.IsFinite()) {
        // alpha * n: multiply leading coef by n, keep rest
        uint64_t n = other.FiniteValue();
        if (n == 0) return Zero();
        lab2::DynamicArray<Term> result;
        int lhs_count = terms_.GetSize();
        for (int i = 0; i < lhs_count; ++i) {
            Term nt;
            nt.exponent = new Ordinal(*terms_[i].exponent);
            nt.coef = (i == 0) ? terms_[0].coef * n : terms_[i].coef;
            AppendTerm(result, nt);
        }
        return FromTerms(result);
    }

    Ordinal result = Zero();
    int other_count = other.terms_.GetSize();
    for (int i = 0; i < other_count; ++i) {
        const Term& ot = other.terms_[i];
        const Ordinal& ot_exp = *ot.exponent;
        uint64_t ot_c = ot.coef;
        Ordinal piece;
        if (ot_exp.IsZero()) {
            piece = *this * Finite(ot_c);
        } else {
            lab2::DynamicArray<Term> terms;
            if (IsFinite()) {
                Term t; t.exponent = new Ordinal(ot_exp); t.coef = ot_c;
                AppendTerm(terms, t);
            } else {
                Ordinal new_exp = *terms_[0].exponent + ot_exp;
                Term t; t.exponent = new Ordinal(new_exp); t.coef = ot_c;
                AppendTerm(terms, t);
            }
            piece = FromTerms(terms);
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
    lab2::DynamicArray<Ordinal::Term> result;
    int term_count = e.Terms().GetSize();
    for (int i = 0; i < term_count; ++i) {
        const Term& t = e.Terms()[i];
        Term nt;
        nt.exponent = new Ordinal(*t.exponent);
        nt.coef = t.coef;
        AppendTerm(result, nt);
    }

    if (result.GetSize() > 0 && result[result.GetSize() - 1].exponent->IsZero()) {
        Term& back = result[result.GetSize() - 1];
        if (back.coef > 1) {
            back.coef -= 1;
        } else {
            delete back.exponent;
            result.Resize(result.GetSize() - 1);
        }
        return Ordinal::FromTerms(result);
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
    lab2::DynamicArray<Term> alpha_terms;
    int exp_count = exp.Terms().GetSize();
    for (int i = 0; i < exp_count; ++i) {
        const Term& t = exp.Terms()[i];
        if (t.exponent->IsZero()) {
            r_val = t.coef;
        } else {
            Ordinal new_exp = ShiftExpDown(*t.exponent);
            Term nt; nt.exponent = new Ordinal(new_exp); nt.coef = t.coef;
            AppendTerm(alpha_terms, nt);
        }
    }
    Ordinal alpha = FromTerms(alpha_terms);

    Ordinal fin_pow = IntPow(r_val);

    if (alpha.IsZero()) {
        return fin_pow;
    }

    Ordinal base_pow;
    if (IsFinite()) {
        {
            lab2::DynamicArray<Term> ts;
            Term t;
            t.exponent = new Ordinal(alpha);
            t.coef = 1;
            AppendTerm(ts, t);
            base_pow = Ordinal::FromTerms(ts);
        }
    } else {
        Ordinal a = *terms_[0].exponent;
        Ordinal a_times_alpha = a * Ordinal::Omega() * alpha;
        lab2::DynamicArray<Term> ts;
        Term t; t.exponent = new Ordinal(a_times_alpha); t.coef = 1;
        AppendTerm(ts, t);
        base_pow = FromTerms(ts);
    }

    return base_pow * fin_pow;
}

std::string Ordinal::ToString() const {
    if (IsZero()) return "0";
    std::string s;
    int count = terms_.GetSize();
    for (int i = 0; i < count; ++i) {
        if (i > 0) s += " + ";
        const Ordinal& e = *terms_[i].exponent;
        uint64_t c = terms_[i].coef;
        if (e.IsZero()) {
            s += std::to_string(c);
        } else if (e == Finite(1)) {
            s += (c == 1) ? "w" : ("w*" + std::to_string(c));
        } else {
            std::string es = e.ToString();
            bool need_paren = (e.Terms().GetSize() > 1);
            if (need_paren) es = "(" + es + ")";
            s += "w^" + es;
            if (c > 1) s += "*" + std::to_string(c);
        }
    }
    return s;
}

}  // namespace lab4
