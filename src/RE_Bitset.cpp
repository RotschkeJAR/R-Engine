#include "RE_Internal.hpp"

namespace RE {

#define CHUNK_SIZE CHAR_BIT
	
	Bitset::BitReference::BitReference(unsigned char *const pmBitmask, const size_t sBitIndex) : pmBitmask(pmBitmask), sBitIndex(sBitIndex) {}
	Bitset::BitReference::BitReference(const BitReference &rCopy) : pmBitmask(rCopy.pmBitmask), sBitIndex(rCopy.sBitIndex) {}
	Bitset::BitReference::~BitReference() {}

	void Bitset::BitReference::flip() {
		*this = not static_cast<bool>(*this);
	}

	Bitset::BitReference::operator bool() const {
		return (*pmBitmask & (1 << sBitIndex));
	}

	void Bitset::BitReference::operator =(bool bNewValue) {
		if (bNewValue)
			*pmBitmask |= 1 << sBitIndex;
		else
			*pmBitmask &= ~(1 << sBitIndex);
	}

	bool Bitset::BitReference::operator ==(const BitReference &rOther) const {
		return pmBitmask == rOther.pmBitmask and sBitIndex == rOther.sBitIndex;
	}

	bool Bitset::BitReference::operator !=(const BitReference &rOther) const {
		return not (*this == rOther);
	}

	std::ostream& operator <<(std::ostream &rStream, const Bitset::BitReference &rBitReference) {
		rStream << static_cast<bool>(rBitReference);
		return rStream;
	}


	Bitset::Bitset() : sBitSize(0) {}
	Bitset::Bitset(const size_t sBitSize, const bool bInitialState) : std_bitArray(std::make_unique<unsigned char[]>(sBitSize / CHUNK_SIZE + 1)), sBitSize(sBitSize) {
		fill(bInitialState);
	}
	Bitset::Bitset(Bitset &rrCopy) : std_bitArray(std::move(rrCopy.std_bitArray)), sBitSize(rrCopy.sBitSize) {
		rrCopy.sBitSize = 0;
	}
	Bitset::~Bitset() {}

	void Bitset::fill(const bool bNewState) {
		std::fill(std_bitArray.get(), std_bitArray.get() + sBitSize / CHUNK_SIZE + 1, bNewState ? UCHAR_MAX : 0);
	}

	void Bitset::swap(Bitset &rOther) {
		std::swap(std_bitArray, rOther.std_bitArray);
		std::swap(sBitSize, rOther.sBitSize);
	}

	void Bitset::resize(const size_t sNewBitSize, const bool bInitialState) {
		std::unique_ptr<unsigned char[]> std_newBitArray = std::make_unique<unsigned char[]>(sNewBitSize / CHUNK_SIZE + 1);
		std::copy(std_bitArray.get(), std_bitArray.get() + std::min(sBitSize, sNewBitSize) / CHUNK_SIZE + 1, std_newBitArray.get());
		if (sNewBitSize > sBitSize) {
			std::fill(std_newBitArray.get() + sBitSize / CHUNK_SIZE + 1, std_newBitArray.get() + sNewBitSize / CHUNK_SIZE + 1, bInitialState ? 0xFF: 0);
			for (size_t sIndex = sBitSize % CHUNK_SIZE; sIndex < CHUNK_SIZE; sIndex++)
				if (bInitialState)
					std_newBitArray[sBitSize / CHUNK_SIZE] |= 1 << sIndex;
				else
					std_newBitArray[sBitSize / CHUNK_SIZE] &= ~(1 << sIndex);
		}
		sBitSize = sNewBitSize;
		std_bitArray = std::move(std_newBitArray);
	}

	void Bitset::clear() {
		std_bitArray.reset();
		sBitSize = 0;
	}

	size_t Bitset::size() const {
		return sBitSize;
	}

	bool Bitset::empty() const {
		return not (static_cast<bool>(std_bitArray) && sBitSize);
	}

	Bitset::BitReference Bitset::at(const size_t sIndex) {
		if (sIndex < sBitSize)
			return (*this)[sIndex];
		RE_ABORT("Bit index ", sIndex, " is out of bounds ([0, ", sBitSize, "[) or bitset is empty");
	}

	Bitset::BitReference Bitset::operator [](const size_t sIndex) {
		return Bitset::BitReference(&std_bitArray[sIndex / CHUNK_SIZE], sIndex % CHUNK_SIZE);
	}

	std::ostream& operator <<(std::ostream &rStream, const Bitset &rBitset) {
		rStream << '{';
		for (size_t sIndex = 0; sIndex < rBitset.sBitSize; sIndex++) {
			if (sIndex != 0)
				rStream << ", ";
			rStream << ((rBitset.std_bitArray[sIndex / CHUNK_SIZE] & (1 << (sIndex % CHUNK_SIZE))) != 0 ? "true" : "false");
		}
		rStream << '}';
		return rStream;
	}

}
