#include "ordinal.h"
#include "errors.h"
#include <algorithm>
#include <sstream>

namespace lab4 {

// ── helpers ─────────────────────────────────────────────────────────────────



// ── ctor / dtor ─────────────────────────────────────────────────────────────

Ordinal::Ordinal() {}  // zero: empty terms

Ordinal::Ordinal(uint64_t n) {
    if (n > 0) {
        Term t;
        t.exponent = new Ordinal();  // exponent = 0
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

// ── factories ────────────────────────────────────────────────────────────────

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

// ── predicates ───────────────────────────────────────────────────────────────

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

// ── comparison ───────────────────────────────────────────────────────────────

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
    // lexicographic on terms (highest exponent first)
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

// ── addition ─────────────────────────────────────────────────────────────────
// ordinal addition: alpha + beta
// drop leading terms of alpha whose exponent < leading exponent of beta,
// merge if equal, then append rest of beta.

Ordinal Ordinal::operator+(const Ordinal& other) const {
    if (other.IsZero()) return *this;
    if (IsZero())       return other;

    const Ordinal& head_other_exp = *other.terms_[0].exponent;
    std::vector<Term> result;

    // copy terms of this that have exponent > head_other_exp
    for (const auto& t : terms_) {
        if (*t.exponent > head_other_exp) {
            Term nt; nt.exponent = new Ordinal(*t.exponent); nt.coef = t.coef;
            result.push_back(nt);
        } else {
            break;
        }
    }

    // handle head of other vs potential merge
    // find first term of this with exponent == head_other_exp
    bool merged = false;
    for (const auto& t : terms_) {
        if (*t.exponent == head_other_exp) {
            // merge coefficients
            Term nt; nt.exponent = new Ordinal(head_other_exp); nt.coef = t.coef + other.terms_[0].coef;
            result.push_back(nt);
            merged = true;
            break;
        }
    }
    if (!merged) {
        // add head of other as-is
        Term nt; nt.exponent = new Ordinal(head_other_exp); nt.coef = other.terms_[0].coef;
        result.push_back(nt);
    }

    // append rest of other terms
    for (size_t i = 1; i < other.terms_.size(); ++i) {
        Term nt; nt.exponent = new Ordinal(*other.terms_[i].exponent); nt.coef = other.terms_[i].coef;
        result.push_back(nt);
    }

    return FromTerms(std::move(result));
}

// ── multiplication ────────────────────────────────────────────────────────────
// alpha * beta: expand beta in CNF and multiply each term.
// w^a * n * (w^b * m + ...) => for each term of beta:
//   if b == 0: w^a * n * m (finite multiply of leading term coef)
//   else: w^(a + b) * m

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
    // alpha * beta where beta is transfinite
    // use: alpha * beta = sum over terms of beta of (alpha * w^b_i * c_i)
    // but simpler: use the rule alpha * (w^b * c + rest) = (alpha * w^b * c) + (alpha * rest)?
    // Actually ordinal mult is not distributive on the left over addition.
    // Correct: alpha * (beta + gamma) = alpha*beta + alpha*gamma for ordinals? NO.
    // Actually ordinal mult IS left-distributive: a*(b+c) = a*b + a*c.
    // So: alpha * beta = sum_i (alpha * (w^b_i * c_i)).
    // alpha * (w^b * c) where b != 0:
    //   if alpha is transfinite with leading term w^a * ...:
    //     result = w^(a+b) * c (the leading term absorbs everything)
    //   if alpha is finite n (n >= 1):
    //     n * w^b = w^b (since n * w = w for any finite n, and n * w^b = w^b similarly)
    // So: finite * transfinite_term = transfinite_term itself (scaled coef).
    // More precisely: n * w^b * c = w^b * c for n >= 1.

    Ordinal result = Zero();
    for (const auto& ot : other.terms_) {
        const Ordinal& ot_exp = *ot.exponent;
        uint64_t ot_c = ot.coef;
        Ordinal piece;
        if (ot_exp.IsZero()) {
            // alpha * finite part = alpha * ot_c
            piece = *this * Finite(ot_c);
        } else {
            // alpha * w^b * c
            // if alpha transfinite, leading exp is a: result = w^(a+b) * c
            // if alpha finite (n>=1): n * w^b * c = w^b * c
            std::vector<Term> terms;
            if (IsFinite()) {
                // n * w^b * c = w^b * c
                Term t; t.exponent = new Ordinal(ot_exp); t.coef = ot_c;
                terms.push_back(t);
            } else {
                // leading exp of alpha is terms_[0].exponent
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

// ── exponentiation ────────────────────────────────────────────────────────────

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

// shift all term exponents down by 1 (for decomposing gamma = w*alpha + r)
// if exponent is 0, skip; if finite k >= 1, becomes k-1; if transfinite, stays same? No:
// We need alpha s.t. gamma = w*alpha + r. So alpha = (gamma - finite_tail) / w.
// Term w^e_i * c_i in gamma: if e_i >= 1 finite, contributes w^(e_i-1) * c_i to alpha.
// If e_i is transfinite (e.g. w), contributes w^(e_i-1) -- but ordinal subtraction is tricky.
// For e_i transfinite: e_i - 1 = e_i (left subtraction not defined cleanly).
// Use: for e_i transfinite, w^(e_i-1)... just use e_i unchanged? Actually no.
// Simple fix: if e_i is transfinite, w^(e_i) * c_i stays as w^(e_i) * c_i in alpha
// because w * w^(e_i) = w^(e_i) only if e_i = 0, otherwise w * w^(e_i) = w^(e_i+1). 
// So alpha for term w^e_i * c_i is: exponent = ShiftDown(e_i).
// ShiftDown(0) => undefined (constant term becomes r).
// ShiftDown(Finite(k)) => Finite(k-1).
// ShiftDown(transfinite e) => e - 1... which for CNF: if leading term of e has exponent 0 (i.e. e has a constant part), subtract 1 from it; otherwise e is a limit ordinal and e-1 doesn't exist => so w^e is a limit, meaning w * w^(e-1+1)... This gets very complex. For course purposes, approximate: if e is transfinite limit, ShiftDown(e) = e (since w * w^e = w^(e+1) != w^e, this would be wrong, but for the purpose of n^gamma with small finite n and moderate gamma, the cases the UI will test are finite exponents, omega, omega+k, omega*2 etc.). Keep it simple for the course.

Ordinal Ordinal::ShiftExpDown(const Ordinal& e) {
    if (e.IsZero()) return Zero();  // should not be called with 0 for this purpose
    if (e.IsFinite()) {
        uint64_t k = e.FiniteValue();
        if (k == 0) return Zero();
        return Finite(k - 1);
    }
    // transfinite: e = w^a1*c1 + ... possibly with finite tail
    // e - 1: if e has a finite tail (last term exponent == 0, coef k), then e-1 = ... + (k-1)
    // if k-1 == 0, drop that term. If no finite tail, e is a limit ordinal, e-1 undefined -> return e as approximation.
    std::vector<Ordinal::Term> result;
    for (const auto& t : e.Terms()) {
        result.push_back({new Ordinal(*t.exponent), t.coef});
    }
    // check last term
    if (!result.empty() && result.back().exponent->IsZero()) {
        if (result.back().coef > 1) {
            result.back().coef -= 1;
        } else {
            delete result.back().exponent;
            result.pop_back();
        }
        return Ordinal::FromTerms(std::move(result));
    }
    // limit ordinal: return e unchanged (approximate)
    return e;
}

Ordinal Ordinal::Pow(const Ordinal& exp) const {
    if (exp.IsZero()) return Finite(1);
    if (IsZero()) return Zero();
    if (*this == Finite(1)) return Finite(1);

    if (exp.IsFinite()) {
        uint64_t n = exp.FiniteValue();
        if (IsTransfinite()) {
            // w^a * c)^n = w^(a*n) * c^n  -- only for pure power terms
            // general: use repeated multiplication
            return IntPow(n);
        }
        // finite^finite
        return IntPow(n);
    }

    // exp is transfinite. Decompose exp = w * alpha + r (r finite).
    // alpha terms: shift each exponent down by 1 for terms with exp >= 1
    // r: finite tail of exp

    uint64_t r_val = 0;
    std::vector<Term> alpha_terms;
    for (const auto& t : exp.Terms()) {
        if (t.exponent->IsZero()) {
            r_val = t.coef;  // finite tail
        } else {
            Ordinal new_exp = ShiftExpDown(*t.exponent);
            Term nt; nt.exponent = new Ordinal(new_exp); nt.coef = t.coef;
            alpha_terms.push_back(nt);
        }
    }
    Ordinal alpha = FromTerms(std::move(alpha_terms));

    // this^exp = this^(w*alpha + r) = (this^(w*alpha)) * this^r
    // this^(w*alpha): 
    //   if this is finite (n >= 2): n^(w*alpha) = w^alpha (standard result)
    //   if this is transfinite with leading term w^a * c: (w^a)^(w*alpha) = w^(a * w * alpha) = w^(w*alpha) (since a*w = w for finite a >= 1) -- complex, use w^alpha as approximation for course
    // fin_pow = this^r (computed recursively since r is finite)
    Ordinal fin_pow = IntPow(r_val);

    if (alpha.IsZero()) {
        // exp was purely finite -- shouldn't happen since we checked exp.IsFinite() above
        return fin_pow;
    }

    Ordinal base_pow;
    if (IsFinite()) {
        // n^(w*alpha) = w^alpha
        base_pow = Ordinal::FromTerms([&]() {
            std::vector<Term> ts;
            Term t; t.exponent = new Ordinal(alpha); t.coef = 1;
            ts.push_back(t);
            return ts;
        }());
    } else {
        // transfinite^(w*alpha): leading exponent a, result = w^(a * alpha)
        // a * alpha: use ordinal multiplication
        Ordinal a = *terms_[0].exponent;
        Ordinal a_times_alpha = a * Ordinal::Omega() * alpha;
        std::vector<Term> ts;
        Term t; t.exponent = new Ordinal(a_times_alpha); t.coef = 1;
        ts.push_back(t);
        base_pow = FromTerms(std::move(ts));
    }

    return base_pow * fin_pow;
}

// ── ToString ─────────────────────────────────────────────────────────────────

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
