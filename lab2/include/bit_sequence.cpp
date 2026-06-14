#include "bit_sequence.h"

namespace lab2 {

class BitSequence::BitEnumerator : public IEnumerator<Bit> {
public:
    BitEnumerator(const BitSequence* owner) : owner_(owner), index_(-1) {}

    bool MoveNext() override {
        if (index_ + 1 >= owner_->GetLength()) {
            return false;
        }
        ++index_;
        cached_ = Bit(owner_->GetBit(index_));
        return true;
    }

    void Reset() override { index_ = -1; }

    Bit Current() const override {
        if (index_ < 0 || index_ >= owner_->GetLength()) {
            throw IndexOutOfRange("enumerator out of range");
        }
        return cached_;
    }

private:
    const BitSequence* owner_;
    int index_;
    Bit cached_;
};

int BitSequence::WordCount(int length) {
    if (length <= 0) {
        return 0;
    }
    return (length + kBitsPerWord - 1) / kBitsPerWord;
}

void BitSequence::MaskTail() {
    int tail = length_ % kBitsPerWord;
    if (tail == 0 || words_.GetSize() == 0) {
        return;
    }
    unsigned int mask = (1u << tail) - 1u;
    int last = words_.GetSize() - 1;
    words_.Set(last, words_.Get(last) & mask);
}

void BitSequence::EnsureIndexValid(int index) const {
    if (index < 0 || index >= length_) {
        throw IndexOutOfRange("bit index out of range");
    }
}

void BitSequence::EnsureSameLength(const BitSequence& other) const {
    if (length_ != other.length_) {
        throw SizeMismatch("bit sequences must have the same length");
    }
}

BitSequence::BitSequence(int length)
    : words_(WordCount(length)), length_(length < 0 ? 0 : length) {
    for (int i = 0; i < words_.GetSize(); ++i) {
        words_.Set(i, 0u);
    }
}

BitSequence::BitSequence(const bool* bits, int length)
    : words_(WordCount(length)), length_(length < 0 ? 0 : length) {
    if (length_ > 0 && bits == nullptr) {
        throw InvalidArgument("bits pointer is null");
    }
    for (int i = 0; i < words_.GetSize(); ++i) {
        words_.Set(i, 0u);
    }
    for (int i = 0; i < length_; ++i) {
        SetBit(i, bits[i]);
    }
}

BitSequence::BitSequence(const std::string& binary)
    : words_(WordCount(static_cast<int>(binary.size()))),
      length_(static_cast<int>(binary.size())) {
    for (int i = 0; i < words_.GetSize(); ++i) {
        words_.Set(i, 0u);
    }
    for (int i = 0; i < length_; ++i) {
        if (binary[i] != '0' && binary[i] != '1') {
            throw InvalidArgument("binary string must contain only 0/1");
        }
        SetBit(i, binary[i] == '1');
    }
}

BitSequence::BitSequence(const BitSequence& other)
    : words_(other.words_), length_(other.length_) {}

BitSequence& BitSequence::operator=(const BitSequence& other) {
    if (this != &other) {
        words_ = other.words_;
        length_ = other.length_;
    }
    return *this;
}

int BitSequence::GetLength() const { return length_; }

Bit BitSequence::GetFirst() const {
    if (length_ == 0) {
        throw EmptyContainer("bit sequence is empty");
    }
    return Bit(GetBit(0));
}

Bit BitSequence::GetLast() const {
    if (length_ == 0) {
        throw EmptyContainer("bit sequence is empty");
    }
    return Bit(GetBit(length_ - 1));
}

Bit BitSequence::Get(int index) const {
    EnsureIndexValid(index);
    return Bit(GetBit(index));
}

bool BitSequence::GetBit(int index) const {
    EnsureIndexValid(index);
    unsigned int word = words_.Get(index / kBitsPerWord);
    return ((word >> (index % kBitsPerWord)) & 1u) != 0u;
}

void BitSequence::SetBit(int index, bool value) {
    EnsureIndexValid(index);
    int w = index / kBitsPerWord;
    unsigned int bit = 1u << (index % kBitsPerWord);
    unsigned int word = words_.Get(w);
    if (value) {
        word |= bit;
    } else {
        word &= ~bit;
    }
    words_.Set(w, word);
}

Sequence<Bit>* BitSequence::GetSubsequence(int start_index, int end_index) const {
    if (start_index < 0 || end_index >= length_ || start_index > end_index) {
        throw IndexOutOfRange("invalid subsequence range");
    }
    int len = end_index - start_index + 1;
    BitSequence* result = new BitSequence(len);
    for (int i = 0; i < len; ++i) {
        result->SetBit(i, GetBit(start_index + i));
    }
    return result;
}

Sequence<Bit>* BitSequence::Append(const Bit& item) {
    const int old_words = WordCount(length_);
    const int new_words = WordCount(length_ + 1);
    if (new_words > old_words) {
        words_.Resize(new_words);
        words_.Set(new_words - 1, 0u);
    }
    ++length_;
    SetBit(length_- 1, item.Value());
    return this;
}

Sequence<Bit>* BitSequence::Prepend(const Bit& item) {
    Append(Bit(false));
    for (int i = length_ - 1; i > 0; --i) {
        SetBit(i, GetBit(i - 1));
    }
    SetBit(0, item.Value());
    return this;
}

Sequence<Bit>* BitSequence::InsertAt(const Bit& item, int index) {
    if (index < 0 || index > length_) {
        throw IndexOutOfRange("insert index out of range");
    }
    if (index == length_) {
        return Append(item);
    }
    Append(Bit(false));
    for (int i = length_ - 1; i > index; --i) {
        SetBit(i, GetBit(i - 1));
    }
    SetBit(index, item.Value());
    return this;
}

Sequence<Bit>* BitSequence::Set(int index, const Bit& value) {
    SetBit(index, value.Value());
    return this;
}

Sequence<Bit>* BitSequence::Concat(const Sequence<Bit>* other) {
    if (other == nullptr) {
        throw InvalidArgument("other sequence is null");
    }
    const int old_length = length_;
    const int add = other->GetLength();
    const int old_words = WordCount(old_length);
    const int new_words = WordCount(old_length + add);
    if (new_words > old_words) {
        words_.Resize(new_words);
        for (int i = old_words; i < new_words; ++i) {
            words_.Set(i, 0u);
        }
    }
    length_ = old_length + add;
    for (int i = 0; i < add; ++i) {
        SetBit(old_length + i, other->Get(i).Value());
    }
    return this;
}

Sequence<Bit>* BitSequence::Clone() const { return new BitSequence(*this); }

IEnumerator<Bit>* BitSequence::GetEnumerator() const {
    return new BitEnumerator(this);
}

BitSequence BitSequence::And(const BitSequence& other) const {
    EnsureSameLength(other);
    BitSequence result(length_);
    for (int i = 0; i < words_.GetSize(); ++i) {
        result.words_.Set(i, words_.Get(i) & other.words_.Get(i));
    }
    result.MaskTail();
    return result;
}

BitSequence BitSequence::Or(const BitSequence& other) const {
    EnsureSameLength(other);
    BitSequence result(length_);
    for (int i = 0; i < words_.GetSize(); ++i) {
        result.words_.Set(i, words_.Get(i) | other.words_.Get(i));
    }
    result.MaskTail();
    return result;
}

BitSequence BitSequence::Xor(const BitSequence& other) const {
    EnsureSameLength(other);
    BitSequence result(length_);
    for (int i = 0; i < words_.GetSize(); ++i) {
        result.words_.Set(i, words_.Get(i) ^ other.words_.Get(i));
    }
    result.MaskTail();
    return result;
}

BitSequence BitSequence::Not() const {
    BitSequence result(length_);
    for (int i = 0; i < words_.GetSize(); ++i) {
        result.words_.Set(i, ~words_.Get(i));
    }
    result.MaskTail();
    return result;
}

bool BitSequence::Equals(const BitSequence& other) const {
    if (length_ != other.length_) {
        return false;
    }
    for (int i = 0; i < words_.GetSize(); ++i) {
        if (words_.Get(i) != other.words_.Get(i)) {
            return false;
        }
    }
    return true;
}

std::string BitSequence::ToString() const {
    std::string result;
    result.reserve(length_);
    for (int i = 0; i < length_; ++i) {
        result.push_back(GetBit(i) ? '1' : '0');
    }
    return result;
}

}  // namespace lab2
