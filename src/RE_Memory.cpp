#include "RE_Internal Header.hpp"

namespace RE {

	void* safe_malloc(const size_t size) {
		void *const pMemory = std::malloc(size);
		if (!pMemory)
			RE_ABORT("Failed to allocate ", size, " bytes of memory with 'malloc'.");
		return pMemory;
	}

	void* safe_align(const size_t alignment, const size_t size, void *&rpPointer, size_t &rSpace) {
		if (rSpace < size)
			RE_WARNING("The remaining memory space is smaller than the required size");
		if (rSpace < alignment)
			RE_WARNING("The remaining memory space is smaller than the required alignment");
		if (!is_multiple_of<size_t>(alignment, 2))
			RE_ABORT("The alignment is not a multiple of 2");
		if (!std::align(alignment, size, rpPointer, rSpace)) {
			RE_ERROR("Failed to align the pointer, because the remaining space is too small");
			return nullptr;
		}
		return rpPointer;
	}

	void* align_2(const size_t alignment, const size_t size, void *&rpPointer, size_t &rSpace) {
		void *const pPointerToAlignedMemory = std::align(alignment, size, rpPointer, rSpace);
		rpPointer = static_cast<uint8_t*>(rpPointer) + size;
		rSpace -= size;
		return pPointerToAlignedMemory;
	}

	void* safe_align_2(const size_t alignment, const size_t size, void *&rpPointer, size_t &rSpace) {
		void *const pPointerToAlignedMemory = safe_align(alignment, size, rpPointer, rSpace);
		if (!pPointerToAlignedMemory)
			return nullptr;
		rpPointer = static_cast<uint8_t*>(rpPointer) + size;
		rSpace -= size;
		return pPointerToAlignedMemory;
	}

}
