#ifndef LAB2_BIT_SEQUENCE_H
#define LAB2_BIT_SEQUENCE_H

#include <string>

#include "bit.h"
#include "dynamic_array.h"
#include "sequence.h"

namespace lab2 {

class BitSequence : public Sequence<Bit> {
public:
    static const unsigned int kBitsPerWord = 8 * sizeof(unsigned int);

    explicit BitSequence(int length = 0);
    BitSequence(const bool* bits, int length);
    explicit BitSequence(const std::string& binary);
    BitSequence(const BitSequence& other);
    BitSequence& operator=(const BitSequence& other);
    ~BitSequence() override = default;

    int GetLength() const override;

    Bit GetFirst() const override;
    Bit GetLast() const override;
    Bit Get(int index) const override;

    bool GetBit(int index) const;
    void SetBit(int index, bool value);

    Sequence<Bit>* GetSubsequence(int start_index, int end_index) const override;

    Sequence<Bit>* Append(const Bit& item) override;
    Sequence<Bit>* Prepend(const Bit& item) override;
    Sequence<Bit>* InsertAt(const Bit& item, int index) override;
    Sequence<Bit>* Set(int index, const Bit& value) override;
    Sequence<Bit>* Concat(const Sequence<Bit>* other) override;

    Sequence<Bit>* Clone() const override;

    IEnumerator<Bit>* GetEnumerator() const override;

    BitSequence And(const BitSequence& other) const;
    BitSequence Or(const BitSequence& other) const;
    BitSequence Xor(const BitSequence& other) const;
    BitSequence Not() const;

    bool Equals(const BitSequence& other) const;
    std::string ToString() const;

private:
    static int WordCount(int length);
    void MaskTail();
    void EnsureIndexValid(int index) const;
    void EnsureSameLength(const BitSequence& other) const;

    DynamicArray<unsigned int> words_;
    int length_;

    class BitEnumerator;
};

}  // namespace lab2

#endif
