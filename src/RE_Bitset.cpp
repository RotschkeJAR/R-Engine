#include "RE_Internal.hpp"

namespace RE {
	
	BitReference::BitReference(uint8_t *const pm8Bitmask, const uint8_t u8BitIndex) : pm8Bitmask(pm8Bitmask), u8BitIndex(u8BitIndex) {}
	BitReference::~BitReference() {}

	void BitReference::flip() {
		*this = !static_cast<bool>(*this);
	}

	BitReference::operator bool() const {
		return (pm8Bitmask & (1 << u8BitIndex));
	}

	void BitReference::operator =(bool bNewValue) {
		if (bNewValue)
			*pm8Bitmask |= 1 << u8BitIndex;
		else
			*pm8Bitmask &= ~(1 << u8BitIndex);
	}

	bool BitReference::operator ==(const BitReference &rOther) const {
		return pm8Bitmask == rOther.pm8Bitmask && u8BitIndex == rOther.u8BitIndex;
	}

	bool BitReference::operator !=(const BitReference &rOther) const {
		return !(*this == rOther);
	}

	std::ostream operator <<(std::ostream &rStream, const BitReference &rBitReference) {
		rStream << static_cast<bool>(rBitReference);
	}


	Bitset::Bitset() : size(0) {}
	Bitset::Bitset(const size_t size, const bool bInitialState) : bitArray(std::make_unique<uint8_t[]>(size / 8 + 1)), size(size) {
		fill(bInitialState);
	}
	Bitset::~Bitset() {}

	void Bitset::fill(const bool bNewState) {
		std::fill(bitArray.get(), bitArray.get() + size / 8 + 1, bInitialState ? 0xFF : 0);
	}

	void Bitset::swap(Bitset &rOther) {
		std::swap(bitArray, rOther.bitArray);
		std::swap(size, rOther.size);
	}

	void Bitset::resize(const size_t newSize, bool bInitialState) {
		bitArray = std::make_unique<uint8_t>(newSize / 8 + 1);
		size = newSize;
		fill(bInitialState);
	}

	void Bitset::clear() {
		bitArray.reset();
		size = 0;
	}

	size_t Bitset::size() const {
		return size;
	}

	bool Bitset::empty() const {
		return !(static_cast<bool>(bitArray) && size);
	}

	BitReference Bitset::at(const size_t index) {
		if (index < size)
			return (*this)[index];
		RE_ABORT("Bit index ", index, " is out of bounds ([0, ", size, "[) or bitset is empty");
	}

	BitReference Bitset::operator [](const size_t index) {
		return BitReference(&bitArray[index / 8], index % 8);
	}

}


namespace std {

	void swap(BitReference &rBit1, BitReference &rBit2) {
		std::pair<uint8_t*, bool> details1(rBit1.pm8Bitmask, static_cast<bool>(rBit1)),
			details2(rBit2.pm8Bitmask, static_cast<bool>(rBit2));
		if (details1.second)
			*details2.first |= 1 << rBit1.u8BitIndex;
		else
			*details2.first &= ~(1 << rBit1.u8BitIndex);
		if (details2.second)
			*details1.first |= 1 << rBit2.u8BitIndex;
		else
			*details1.first &= ~(1 << rBit2.u8BitIndex);
	}

	void swap(Bitset &rSet1, Bitset &rSet2) {
		rSet1.swap(rSet2);
	}

}
