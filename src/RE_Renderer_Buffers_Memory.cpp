#include "RE_Renderer_Buffers_Internal.hpp"

namespace RE {

	static std::unique_ptr<VulkanMemory[]> localBufferMemories,
		deviceBufferMemories;
	
	bool alloc_memories_for_buffers_renderer() {
		return false;
	}

	void free_memories_for_buffers_renderer() {

	}

}