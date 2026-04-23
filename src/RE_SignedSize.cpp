#include "RE_Internal.hpp"

namespace RE {
	
	SignedSize::SignedSize() : SignedSize(0) {}
	SignedSize::SignedSize(size_t size) : size(size), bNegative(false) {}
	SignedSize::SignedSize(size_t size, bool bNegative) : size(size), bNegative(bNegative) {}
	SignedSize::SignedSize(SignedSize &rCopy) : size(rCopy.size), bNegative(rCopy.bNegative) {}
	SignedSize::SignedSize(SignedSize &&rrCopy) : size(rrCopy.size), bNegative(rrCopy.bNegative) {
		rrCopy.size = 0;
		rrCopy.bNegative = false;
	}
	SignedSize::~SignedSize() {}

}
