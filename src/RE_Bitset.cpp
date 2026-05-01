#include "RE_Internal Header.hpp"

namespace RE {
	
	Bitset::BitReference::BitReference(uint8_t *const pm8Bitmask, const uint8_t u8BitIndex) : pm8Bitmask(pm8Bitmask), u8BitIndex(u8BitIndex) {}
	Bitset::BitReference::~BitReference() {}

	void Bitset::BitReference::flip() {
		*this = !static_cast<bool>(*this);
	}

	Bitset::BitReference::operator bool() const {
		return (*pm8Bitmask & (1 << u8BitIndex));
	}

	void Bitset::BitReference::operator =(bool bNewValue) {
		if (bNewValue)
			*pm8Bitmask |= 1 << u8BitIndex;
		else
			*pm8Bitmask &= ~(1 << u8BitIndex);
	}

	bool Bitset::BitReference::operator ==(const BitReference &rOther) const {
		return pm8Bitmask == rOther.pm8Bitmask && u8BitIndex == rOther.u8BitIndex;
	}

	bool Bitset::BitReference::operator !=(const BitReference &rOther) const {
		return !(*this == rOther);
	}

	std::ostream& operator <<(std::ostream &rStream, const Bitset::BitReference &rBitReference) {
		rStream << static_cast<bool>(rBitReference);
		return rStream;
	}


	Bitset::Bitset() : bitSize(0) {}
	Bitset::Bitset(const size_t bitSize, const bool bInitialState) : bitArray(std::make_unique<uint8_t[]>(bitSize / 8 + 1)), bitSize(bitSize) {
		fill(bInitialState);
	}
	Bitset::~Bitset() {}

	void Bitset::fill(const bool bNewState) {
		std::fill(bitArray.get(), bitArray.get() + bitSize / 8 + 1, bNewState ? 0xFF : 0);
	}

	void Bitset::swap(Bitset &rOther) {
		std::swap(bitArray, rOther.bitArray);
		std::swap(bitSize, rOther.bitSize);
	}

	void Bitset::resize(const size_t newBitSize, const bool bInitialState) {
		std::unique_ptr<uint8_t[]> newBitArray = std::make_unique<uint8_t[]>(newBitSize / 8 + 1);
		std::copy(bitArray.get(), bitArray.get() + std::min(bitSize, newBitSize) / 8 + 1, newBitArray.get());
		if (newBitSize > bitSize) {
			std::fill(newBitArray.get() + bitSize / 8 + 1, newBitArray.get() + newBitSize / 8 + 1, bInitialState ? 0xFF: 0);
			for (size_t i = bitSize % 8; i < 8; i++)
				if (bInitialState)
					newBitArray[bitSize / 8] |= 1 << i;
				else
					newBitArray[bitSize / 8] &= ~(1 << i);
		}
		bitSize = newBitSize;
		bitArray = std::move(newBitArray);
	}

	void Bitset::clear() {
		bitArray.reset();
		bitSize = 0;
	}

	size_t Bitset::size() const {
		return bitSize;
	}

	bool Bitset::empty() const {
		return !(static_cast<bool>(bitArray) && bitSize);
	}

	Bitset::BitReference Bitset::at(const size_t index) {
		if (index < bitSize)
			return (*this)[index];
		RE_ABORT("Bit index ", index, " is out of bounds ([0, ", bitSize, "[) or bitset is empty");
	}

	Bitset::BitReference Bitset::operator [](const size_t index) {
		return Bitset::BitReference(&bitArray[index / 8], index % 8);
	}

}
