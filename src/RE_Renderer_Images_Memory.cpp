#include "RE_Renderer_Images_Internal.hpp"

namespace RE {
	
	static std::unique_ptr<VulkanMemory[]> imageMemories;
	static std::unique_ptr<VulkanMemory[]> swapchainRelatedImageMemories;

	bool alloc_memory_for_renderer_images() {
#ifdef RE_OS_LINUX
		PRINT_DEBUG("Allocating shared Vulkan memory for images");
		const SharedVulkanMemoryInfo aImageInfos[] = {
			{
				.vulkanStorageObject = vk_hWindowButtonImage,
				.u32RegionIndex = 0
			}
		};
		size_t memoryAllocationCount;
		if (alloc_shared_vulkan_memory(sizeof(aImageInfos) / sizeof(aImageInfos[0]), aImageInfos, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryAllocationCount, imageMemories) == VK_SUCCESS)
			return true;
		else
			RE_FATAL_ERROR("Failed to allocate memory for Vulkan images");
		return false;
#else
		return true;
#endif
	}

	bool alloc_memory_for_swapchain_related_images() {
		PRINT_DEBUG("Allocating shared Vulkan memory for swpachain related images");
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
		if (alloc_shared_vulkan_memory(static_cast<uint32_t>(imageInfos.size()), imageInfos.data(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memoryAllocationCount, swapchainRelatedImageMemories) == VK_SUCCESS)
			return true;
		else
			RE_FATAL_ERROR("Failed to allocate memory for swapchain related Vulkan images");
		return false;
	}

	void free_memory_for_renderer_images() {
		PRINT_DEBUG("Freeing all Vulkan memories occupied by images");
		imageMemories.reset();
	}

	void free_memory_for_swapchain_related_images() {
		PRINT_DEBUG("Freeing all Vulkan memories occupied by swpachain related images");
		swapchainRelatedImageMemories.reset();
	}

}
