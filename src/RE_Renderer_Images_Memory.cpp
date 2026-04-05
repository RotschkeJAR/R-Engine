#include "RE_Renderer_Images_Internal.hpp"

namespace RE {
	
	static VulkanMemory imageMemory;

	bool alloc_memory_for_images_renderer() {
		PRINT_DEBUG("Setting data structures up for querying memory requirements of renderable images");
		VkImageMemoryRequirementsInfo2 vk_imageInfo;
		vk_imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2;
		vk_imageInfo.pNext = nullptr;
		VkMemoryRequirements2 vk_memoryRequirements2;
		vk_memoryRequirements2.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
		vk_memoryRequirements2.pNext = nullptr;
		const VkMemoryRequirements &vk_rMemoryRequirements = vk_memoryRequirements2.memoryRequirements;
		VkDeviceSize vk_memoryToAllocate = 0,
			vk_renderTargetSize,
			vk_depthAlignment,
			vk_depthSize,
			vk_stencilAlignment,
			vk_singlesampledAlignment;
		uint32_t m32AvailableMemoryTypes = 0xFFFFFFFF;

		if (vk_hRenderTargetImage) {
			vk_imageInfo.image = vk_hRenderTargetImage;
			PRINT_DEBUG("Querying memory requirements of Vulkan image ", vk_imageInfo.image, " used as render target");
			vkGetImageMemoryRequirements2(vk_hDevice, &vk_imageInfo, &vk_memoryRequirements2);
			vk_memoryToAllocate += vk_rMemoryRequirements.size;
			vk_renderTargetSize = vk_rMemoryRequirements.size;
			m32AvailableMemoryTypes &= vk_rMemoryRequirements.memoryTypeBits;
		} else
			vk_renderTargetSize = 0;

		vk_imageInfo.image = vk_hDepthImage;
		PRINT_DEBUG("Querying memory requirements of Vulkan image ", vk_imageInfo.image, " used as depth image");
		vkGetImageMemoryRequirements2(vk_hDevice, &vk_imageInfo, &vk_memoryRequirements2);
		vk_depthAlignment = vk_rMemoryRequirements.alignment;
		vk_depthSize = vk_rMemoryRequirements.size;
		vk_memoryToAllocate = next_multiple_inclusive<VkDeviceSize>(vk_memoryToAllocate, vk_depthAlignment) + vk_depthSize;
		m32AvailableMemoryTypes &= vk_rMemoryRequirements.memoryTypeBits;

		const bool bSeparateStencilImagePresent = vk_hDepthImage != vk_hStencilImage && vk_hStencilImage;
		if (bSeparateStencilImagePresent) {
			vk_imageInfo.image = vk_hStencilImage;
			PRINT_DEBUG("Querying memory requirements of Vulkan image ", vk_imageInfo.image, " used as separate stencil image");
			vkGetImageMemoryRequirements2(vk_hDevice, &vk_imageInfo, &vk_memoryRequirements2);
			vk_stencilAlignment = vk_rMemoryRequirements.alignment;
			vk_memoryToAllocate = next_multiple_inclusive<VkDeviceSize>(vk_memoryToAllocate, vk_stencilAlignment) + vk_rMemoryRequirements.size;
			m32AvailableMemoryTypes &= vk_rMemoryRequirements.memoryTypeBits;
		}

		if (vk_hSinglesampledImage) {
			vk_imageInfo.image = vk_hSinglesampledImage;
			PRINT_DEBUG("Querying memory requirements of Vulkan image ", vk_imageInfo.image, " used as singlesampled render target");
			vkGetImageMemoryRequirements2(vk_hDevice, &vk_imageInfo, &vk_memoryRequirements2);
			vk_singlesampledAlignment = vk_rMemoryRequirements.alignment;
			vk_memoryToAllocate = next_multiple_inclusive<VkDeviceSize>(vk_memoryToAllocate, vk_singlesampledAlignment) + vk_rMemoryRequirements.size;
			m32AvailableMemoryTypes &= vk_rMemoryRequirements.memoryTypeBits;
		}

		if (m32AvailableMemoryTypes) {
			PRINT_DEBUG("Allocating Vulkan memory of ", vk_memoryToAllocate, " bytes for renderable images");
			if (imageMemory.alloc(vk_memoryToAllocate, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m32AvailableMemoryTypes) == VK_SUCCESS) {
				PRINT_DEBUG("Setting data structures up for binding Vulkan memory ", imageMemory.get(), " to renderable images");
				std::vector<VkBindImageMemoryInfo> bindInfos;
				bindInfos.reserve(4);
				if (vk_hRenderTargetImage)
					bindInfos.emplace_back(VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO,
							nullptr,
							vk_hRenderTargetImage,
							imageMemory.get(),
							0);
				const VkDeviceSize vk_depthFirstByteIndex = next_multiple_inclusive<VkDeviceSize>(vk_renderTargetSize, vk_depthAlignment);
				bindInfos.emplace_back(VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO,
						nullptr,
						vk_hDepthImage,
						imageMemory.get(),
						vk_depthFirstByteIndex);
				if (bSeparateStencilImagePresent)
					bindInfos.emplace_back(VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO,
							nullptr,
							vk_hStencilImage,
							imageMemory.get(),
							next_multiple_inclusive<VkDeviceSize>(vk_depthFirstByteIndex + vk_depthSize, vk_stencilAlignment));
				if (vk_hSinglesampledImage)
					bindInfos.emplace_back(VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO,
							nullptr,
							vk_hSinglesampledImage,
							imageMemory.get(),
							next_multiple_inclusive<VkDeviceSize>(vk_renderTargetSize, vk_singlesampledAlignment));
				PRINT_DEBUG("Binding Vulkan memory to renderable images");
				const VkResult vk_eResult = vkBindImageMemory2(vk_hDevice, static_cast<uint32_t>(bindInfos.size()), bindInfos.data());
				if (vk_eResult == VK_SUCCESS)
					return true;
				else
					RE_FATAL_ERROR("Failed to bind Vulkan memory ", imageMemory.get(), " to all renderable images (Vulkan error code: ", std::hex, vk_eResult, ")");
				PRINT_DEBUG("Freeing Vulkan memory used for renderable images due to failure binding the memory to the images");
				imageMemory.free();
			} else
				RE_FATAL_ERROR("Failed to allocate Vulkan memory for renderable images");
		} else
			RE_FATAL_ERROR("All renderable images don't share any memory type they allow to allocate memory from");
		return false;
	}

	void free_memory_for_images_renderer() {
		imageMemory.free();
	}

}
