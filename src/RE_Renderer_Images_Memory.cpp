#include "RE_Renderer_Images_Internal.hpp"

namespace RE {
	
	static std::unique_ptr<VulkanMemory[]> imageMemories;

	bool alloc_memory_for_images_renderer() {
		PRINT_DEBUG("Allocating shared Vulkan memory for render images");
		std::vector<SharedVulkanMemoryInfo> imageInfos;
		imageInfos.reserve(4);
		if (vk_hRenderTargetImage)
			imageInfos.emplace_back(vk_hRenderTargetImage, 0);
		if (vk_hSinglesampledImage)
			imageInfos.emplace_back(vk_hSinglesampledImage, 0);
		imageInfos.emplace_back(vk_hDepthImage, 0);
		if (vk_hStencilImage && vk_hStencilImage != vk_hDepthImage)
			imageInfos.emplace_back(vk_hStencilImage, 0);
		size_t memoryAllocationCount;
		if (alloc_shared_vulkan_memory(static_cast<uint32_t>(imageInfos.size()), imageInfos.data(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryAllocationCount, imageMemories) == VK_SUCCESS)
			return true;
		else
			RE_FATAL_ERROR("Failed to allocate memory for all Vulkan images used by the renderer");
		return false;
	}

	void free_memory_for_images_renderer() {
		PRINT_DEBUG("Freeing all Vulkan memories occupied by render images");
		imageMemories.reset();
	}

}
