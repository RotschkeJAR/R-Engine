#include "RE_Renderer_Internal.hpp"
#include "RE_Window.hpp"

namespace RE {
	
	std::array<VkImage, RE_VK_FRAMES_IN_FLIGHT * 2> vk_ahDepthStencilImages;
	static std::array<VkDeviceMemory, vk_ahDepthStencilImages.size()> vk_ahDepthStencilImageMemories;
	std::array<VkImageView, vk_ahDepthStencilImages.size()> vk_ahDepthStencilImageViews;
	static std::vector<VkFormat> availableDepthStencilFormats;
	VkImageTiling vk_eDepthImageTiling, vk_eStencilImageTiling = VK_IMAGE_TILING_MAX_ENUM;
	uint8_t u8IndexToSelectedDepthStencilFormat = 0;
	bool bStencilsEnabled = false;

	static bool are_depth_stencil_images_separated(const VkFormat vk_eFormat) {
		switch (vk_eFormat) {
			case VK_FORMAT_D32_SFLOAT_S8_UINT:
			case VK_FORMAT_D24_UNORM_S8_UINT:
			case VK_FORMAT_D16_UNORM_S8_UINT:
				return false;
			case VK_FORMAT_D32_SFLOAT:
			case VK_FORMAT_X8_D24_UNORM_PACK32:
			case VK_FORMAT_D16_UNORM:
			case VK_FORMAT_S8_UINT:
				return true;
			[[unlikely]] default:
				RE_FATAL_ERROR("Unknown Vulkan format used to determine separation between depth and stencil data");
				std::abort();
				return false;
		}
	}

	static void get_depth_stencil_format_properties(const VkFormat vk_eFormat, VkFormatProperties2 *const vk_pFormatProperties, void *const pNext) {
		vk_pFormatProperties->sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
		vk_pFormatProperties->pNext = pNext;
		vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDeviceSelected, vk_eFormat, vk_pFormatProperties);
	}

	static void get_depth_stencil_image_properties(const VkFormat vk_eDepthStencilFormat, const VkImageTiling vk_eDepthStencilTiling, VkImageFormatProperties2 *const vk_pDepthStencilImageFormatProperties) {
		const VkPhysicalDeviceImageFormatInfo2 vk_physicalDeviceDepthStencilImageFormatInfo = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2,
			.format = vk_eDepthStencilFormat,
			.type = VK_IMAGE_TYPE_2D,
			.tiling = vk_eDepthStencilTiling,
			.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
		};
		vkGetPhysicalDeviceImageFormatProperties2(vk_hPhysicalDeviceSelected, &vk_physicalDeviceDepthStencilImageFormatInfo, vk_pDepthStencilImageFormatProperties);
	}

	static void select_best_depth_stencil_format() {
		
	}

	bool does_gpu_support_depth_stencil_images(const VkPhysicalDevice vk_hPhysicalDevice, std::queue<std::string> &rMissingFeatures) {
		constexpr VkFormat vk_aeDesiredFormats[] = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_S8_UINT,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_X8_D24_UNORM_PACK32,
			VK_FORMAT_D16_UNORM
		};
		bool bSupportsDepthBuffers = false, bSupportsStencilBuffers = false;
		VkFormatProperties2 vk_formatProperties;
		vk_formatProperties.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
		vk_formatProperties.pNext = nullptr;
		VkPhysicalDeviceImageFormatInfo2 vk_physicalDeviceDepthStencilImageFormatInfo;
		vk_physicalDeviceDepthStencilImageFormatInfo.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2;
		vk_physicalDeviceDepthStencilImageFormatInfo.pNext = nullptr;
		vk_physicalDeviceDepthStencilImageFormatInfo.type = VK_IMAGE_TYPE_2D;
		vk_physicalDeviceDepthStencilImageFormatInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		vk_physicalDeviceDepthStencilImageFormatInfo.flags = 0;
		VkImageFormatProperties2 vk_imageFormatProperties;
		vk_imageFormatProperties.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2;
		vk_imageFormatProperties.pNext = nullptr;
		for (const VkFormat vk_eDesiredFormat : vk_aeDesiredFormats) {
			bool bFormatIsSuitable = false;
			vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDevice, vk_eDesiredFormat, &vk_formatProperties);
			vk_physicalDeviceDepthStencilImageFormatInfo.format = vk_eDesiredFormat;
			if ((vk_formatProperties.formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0) {
				vk_physicalDeviceDepthStencilImageFormatInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
				vkGetPhysicalDeviceImageFormatProperties2(vk_hPhysicalDevice, &vk_physicalDeviceDepthStencilImageFormatInfo, &vk_imageFormatProperties);
				if (vk_imageFormatProperties.imageFormatProperties.maxExtent.width >= largestMonitorSize[0] && vk_imageFormatProperties.imageFormatProperties.maxExtent.height >= largestMonitorSize[1])
					bFormatIsSuitable = true;
			}
			if (!bFormatIsSuitable && (vk_formatProperties.formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0) {
				vk_physicalDeviceDepthStencilImageFormatInfo.tiling = VK_IMAGE_TILING_LINEAR;
				vkGetPhysicalDeviceImageFormatProperties2(vk_hPhysicalDevice, &vk_physicalDeviceDepthStencilImageFormatInfo, &vk_imageFormatProperties);
				if (vk_imageFormatProperties.imageFormatProperties.maxExtent.width >= largestMonitorSize[0] && vk_imageFormatProperties.imageFormatProperties.maxExtent.height >= largestMonitorSize[1])
					bFormatIsSuitable = true;
			}
			if (!bFormatIsSuitable)
				continue;
			switch (vk_eDesiredFormat) {
				case VK_FORMAT_D32_SFLOAT_S8_UINT:
				case VK_FORMAT_D24_UNORM_S8_UINT:
				case VK_FORMAT_D16_UNORM_S8_UINT:
					bSupportsDepthBuffers = true;
					bSupportsStencilBuffers = true;
					break;
				case VK_FORMAT_S8_UINT:
					bSupportsStencilBuffers = true;
					break;
				case VK_FORMAT_D16_UNORM:
				case VK_FORMAT_X8_D24_UNORM_PACK32:
				case VK_FORMAT_D32_SFLOAT:
					bSupportsDepthBuffers = true;
					break;
				[[unlikely]] default:
					RE_FATAL_ERROR("Unknown Vulkan format used to distinguish between depth and stencils");
					std::abort();
			}
			if (bSupportsDepthBuffers && bSupportsStencilBuffers)
				return true;
		}
		if (!bSupportsDepthBuffers)
			rMissingFeatures.emplace("Depth images aren't supported or too small for the monitors");
		if (!bSupportsStencilBuffers)
			rMissingFeatures.emplace("Stencil images aren't supported or too small for the monitors");
		return false;
	}

	int32_t rate_gpu_depth_stencil_image_formats(const VkPhysicalDevice vk_hPhysicalDevice) {
		int32_t i32Score = 0;
		VkFormatProperties2 vk_formatProperties;
		vk_formatProperties.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
		vk_formatProperties.pNext = nullptr;
		VkPhysicalDeviceImageFormatInfo2 vk_physicalDeviceDepthStencilImageFormatInfo;
		vk_physicalDeviceDepthStencilImageFormatInfo.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2;
		vk_physicalDeviceDepthStencilImageFormatInfo.pNext = nullptr;
		vk_physicalDeviceDepthStencilImageFormatInfo.type = VK_IMAGE_TYPE_2D;
		vk_physicalDeviceDepthStencilImageFormatInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		vk_physicalDeviceDepthStencilImageFormatInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		vk_physicalDeviceDepthStencilImageFormatInfo.flags = 0;
		VkImageFormatProperties2 vk_imageFormatProperties;
		vk_imageFormatProperties.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2;
		vk_imageFormatProperties.pNext = nullptr;
		constexpr VkFormat vk_aeAllFormats[] = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_S8_UINT,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_X8_D24_UNORM_PACK32,
			VK_FORMAT_D16_UNORM
		};
		bool bSupportsDepthStencilCombined = false, bSupportsStencilsOnly = false, bSupportsDepthOnly = false;
		for (const VkFormat vk_eFormat : vk_aeAllFormats) {
			vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDevice, vk_eFormat, &vk_formatProperties);
			if ((vk_formatProperties.formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0) {
				vk_physicalDeviceDepthStencilImageFormatInfo.format = vk_eFormat;
				vkGetPhysicalDeviceImageFormatProperties2(vk_hPhysicalDevice, &vk_physicalDeviceDepthStencilImageFormatInfo, &vk_imageFormatProperties);
				if (vk_imageFormatProperties.imageFormatProperties.maxExtent.width >= largestMonitorSize[0] && vk_imageFormatProperties.imageFormatProperties.maxExtent.height >= largestMonitorSize[1])
					switch (vk_eFormat) {
						case VK_FORMAT_D32_SFLOAT_S8_UINT:
						case VK_FORMAT_D24_UNORM_S8_UINT:
						case VK_FORMAT_D16_UNORM_S8_UINT:
							if (bSupportsDepthStencilCombined)
								i32Score += 10;
							else {
								bSupportsDepthStencilCombined = true;
								i32Score += 500;
							}
							break;
						case VK_FORMAT_S8_UINT:
							bSupportsStencilsOnly = true;
							break;
						case VK_FORMAT_D32_SFLOAT:
						case VK_FORMAT_X8_D24_UNORM_PACK32:
						case VK_FORMAT_D16_UNORM:
							if (bSupportsDepthOnly)
								i32Score += 10;
							else {
								bSupportsDepthOnly = true;
								i32Score += 500;
							}
							break;
						[[unlikely]] default:
							RE_FATAL_ERROR("Unknown Vulkan format used to rate GPU");
							std::abort();
					}
			}
		}
		if ((!bSupportsDepthStencilCombined && bSupportsStencilsOnly && bSupportsDepthOnly) || (bSupportsDepthStencilCombined && !bSupportsStencilsOnly && !bSupportsDepthOnly))
			i32Score /= 10; // Bad GPU design and likely causes many cache-misses or unnecessary memory allocation
		else if (!bSupportsDepthStencilCombined && !bSupportsStencilsOnly && !bSupportsDepthOnly)
			i32Score = -5000; // Depth-stencil images only available in linear tiling, which likely causes high memory usage and many cache-misses
		return i32Score;
	}

	void find_suitable_depth_stencil_formats() {
		availableDepthStencilFormats.reserve(7);
		VkFormatProperties2 vk_formatProperties;
		vk_formatProperties.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
		vk_formatProperties.pNext = nullptr;
		VkPhysicalDeviceImageFormatInfo2 vk_physicalDeviceImageFormatInfo;
		vk_physicalDeviceImageFormatInfo.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2;
		vk_physicalDeviceImageFormatInfo.pNext = nullptr;
		vk_physicalDeviceImageFormatInfo.format = VK_FORMAT_S8_UINT;
		vk_physicalDeviceImageFormatInfo.type = VK_IMAGE_TYPE_2D;
		vk_physicalDeviceImageFormatInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		vk_physicalDeviceImageFormatInfo.flags = 0;
		VkImageFormatProperties2 vk_imageFormatProperties;
		vk_imageFormatProperties.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2;
		vk_imageFormatProperties.pNext = nullptr;

		vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDeviceSelected, VK_FORMAT_S8_UINT, &vk_formatProperties);
		if ((vk_formatProperties.formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)) {
			vk_physicalDeviceImageFormatInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			vkGetPhysicalDeviceImageFormatProperties2(vk_hPhysicalDeviceSelected, &vk_physicalDeviceImageFormatInfo, &vk_imageFormatProperties);
			if (vk_imageFormatProperties.imageFormatProperties.maxExtent.width >= largestMonitorSize[0] && vk_imageFormatProperties.imageFormatProperties.maxExtent.height >= largestMonitorSize[1])
				vk_eStencilImageTiling = VK_IMAGE_TILING_OPTIMAL;
		}
		if (vk_eStencilImageTiling == VK_IMAGE_TILING_MAX_ENUM && (vk_formatProperties.formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)) {
			vk_physicalDeviceImageFormatInfo.tiling = VK_IMAGE_TILING_LINEAR;
			vkGetPhysicalDeviceImageFormatProperties2(vk_hPhysicalDeviceSelected, &vk_physicalDeviceImageFormatInfo, &vk_imageFormatProperties);
			if (vk_imageFormatProperties.imageFormatProperties.maxExtent.width >= largestMonitorSize[0] && vk_imageFormatProperties.imageFormatProperties.maxExtent.height >= largestMonitorSize[1])
				vk_eStencilImageTiling = VK_IMAGE_TILING_LINEAR;
		}
		for (uint8_t u8AttemptIndex = 0; u8AttemptIndex < 2; u8AttemptIndex++) {
			const VkImageTiling vk_eImageTiling = u8AttemptIndex == 0 ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR;
			constexpr VkFormat vk_aeDepthStencilCombinedFormats[] = {
				VK_FORMAT_D32_SFLOAT_S8_UINT,
				VK_FORMAT_D24_UNORM_S8_UINT,
				VK_FORMAT_D16_UNORM_S8_UINT,
				VK_FORMAT_D32_SFLOAT,
				VK_FORMAT_X8_D24_UNORM_PACK32,
				VK_FORMAT_D16_UNORM
			};
			for (const VkFormat vk_eDepthStencilFormat : vk_aeDepthStencilCombinedFormats) {
				vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDeviceSelected, vk_eDepthStencilFormat, &vk_formatProperties);
				if (u8AttemptIndex == 0 ? (vk_formatProperties.formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) : (vk_formatProperties.formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)) {
					vk_physicalDeviceImageFormatInfo.format = vk_eDepthStencilFormat;
					vkGetPhysicalDeviceImageFormatProperties2(vk_hPhysicalDeviceSelected, &vk_physicalDeviceImageFormatInfo, &vk_imageFormatProperties);
					if (vk_imageFormatProperties.imageFormatProperties.maxExtent.width >= largestMonitorSize[0] && vk_imageFormatProperties.imageFormatProperties.maxExtent.height >= largestMonitorSize[1])
						availableDepthStencilFormats.push_back(vk_eDepthStencilFormat);
				}
			}
			if (!availableDepthStencilFormats.empty()) {
				vk_eDepthImageTiling = vk_eImageTiling;
				break;
			}
		}
		availableDepthStencilFormats.shrink_to_fit();
		u8IndexToSelectedDepthStencilFormat = 0;
	}

	bool create_depth_stencil_buffers(VulkanTask &rDepthStencilImageLayoutTransitionTask, VkFence vk_hDepthStencilImageLayoutTransitionFence) {
		const VkExtent3D vk_depthStencilImageExtent = {
			.width = vk_swapchainResolution.width,
			.height = vk_swapchainResolution.height,
			.depth = 1
		};
		if (!bStencilsEnabled && std::none_of(availableDepthStencilFormats.rbegin(), availableDepthStencilFormats.rend(), 
				[](const VkFormat vk_eFormat) -> bool {
					switch (vk_eFormat) {
						case VK_FORMAT_D32_SFLOAT:
						case VK_FORMAT_X8_D24_UNORM_PACK32:
						case VK_FORMAT_D16_UNORM:
							return true;
						default:
							return false;
					}
				}
		)) {
			RE_NOTE("The selected GPU doesn't support any format to use depth images only. The engine will use a depth-stencil-combined image format instead, which might result in reserving unused memory");
			bStencilsEnabled = true;
		}
		const uint8_t a1u8RenderGraphicsLogicalQueueIndex[1] = {
			get_render_graphics_queue_logical_index()
		};
		PUSH_TO_CALLSTACKTRACE(rDepthStencilImageLayoutTransitionTask.init(1, a1u8RenderGraphicsLogicalQueueIndex, true));
		if (bStencilsEnabled) {
			VkFormat vk_eSelectedDepthFormat = availableDepthStencilFormats[u8IndexToSelectedDepthStencilFormat];
			if (are_depth_stencil_images_separated(vk_eSelectedDepthFormat)) {
				if (vk_eStencilImageTiling != VK_IMAGE_TILING_MAX_ENUM) {
					uint8_t u8RenderInFlightImageCreateIndex = 0;
					while (u8RenderInFlightImageCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
						if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_image(0, VK_IMAGE_TYPE_2D, vk_eSelectedDepthFormat, vk_depthStencilImageExtent, 1, 1, vk_eMsaaCount, vk_eDepthImageTiling, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 1, nullptr, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_ahDepthStencilImages[u8RenderInFlightImageCreateIndex * 2], &vk_ahDepthStencilImageMemories[u8RenderInFlightImageCreateIndex * 2]), bool)) {
							if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_image_view(vk_ahDepthStencilImages[u8RenderInFlightImageCreateIndex * 2], VK_IMAGE_VIEW_TYPE_2D, vk_eSelectedDepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1, &vk_ahDepthStencilImageViews[u8RenderInFlightImageCreateIndex * 2]), bool)) {
								if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_image(0, VK_IMAGE_TYPE_2D, VK_FORMAT_S8_UINT, vk_depthStencilImageExtent, 1, 1, vk_eMsaaCount, vk_eStencilImageTiling, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 1, nullptr, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_ahDepthStencilImages[u8RenderInFlightImageCreateIndex * 2 + 1], &vk_ahDepthStencilImageMemories[u8RenderInFlightImageCreateIndex * 2 + 1]), bool)) {
									if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_image_view(vk_ahDepthStencilImages[u8RenderInFlightImageCreateIndex * 2 + 1], VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_S8_UINT, VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, 0, 1, &vk_ahDepthStencilImageViews[u8RenderInFlightImageCreateIndex * 2 + 1]), bool)) {
										u8RenderInFlightImageCreateIndex++;
										continue;
									} else
										RE_FATAL_ERROR("Failed to create Vulkan image view for stencil image at index ", u8RenderInFlightImageCreateIndex);
									vkFreeMemory(vk_hDevice, vk_ahDepthStencilImageMemories[u8RenderInFlightImageCreateIndex * 2 + 1], nullptr);
									vkDestroyImage(vk_hDevice, vk_ahDepthStencilImages[u8RenderInFlightImageCreateIndex * 2 + 1], nullptr);
								} else
									RE_FATAL_ERROR("Failed to create separated Vulkan stencil image at index ", u8RenderInFlightImageCreateIndex);
								vkDestroyImageView(vk_hDevice, vk_ahDepthStencilImageViews[u8RenderInFlightImageCreateIndex * 2], nullptr);
							} else
								RE_FATAL_ERROR("Failed to create separated Vulkan image view for depth image at index ", u8RenderInFlightImageCreateIndex);
							vkFreeMemory(vk_hDevice, vk_ahDepthStencilImageMemories[u8RenderInFlightImageCreateIndex * 2], nullptr);
							vkDestroyImage(vk_hDevice, vk_ahDepthStencilImages[u8RenderInFlightImageCreateIndex * 2], nullptr);
						} else
							RE_FATAL_ERROR("Failed to create separated Vulkan depth image at index ", u8RenderInFlightImageCreateIndex);
						break;
					}
					if (u8RenderInFlightImageCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
						for (uint8_t u8RenderInFlightImageDestroyIndex = 0; u8RenderInFlightImageDestroyIndex < u8RenderInFlightImageCreateIndex; u8RenderInFlightImageDestroyIndex++) {
							vkDestroyImageView(vk_hDevice, vk_ahDepthStencilImageViews[u8RenderInFlightImageDestroyIndex * 2 + 1], nullptr);
							vkFreeMemory(vk_hDevice, vk_ahDepthStencilImageMemories[u8RenderInFlightImageDestroyIndex * 2 + 1], nullptr);
							vkDestroyImage(vk_hDevice, vk_ahDepthStencilImages[u8RenderInFlightImageDestroyIndex * 2 + 1], nullptr);
							vkDestroyImageView(vk_hDevice, vk_ahDepthStencilImageViews[u8RenderInFlightImageDestroyIndex * 2], nullptr);
							vkFreeMemory(vk_hDevice, vk_ahDepthStencilImageMemories[u8RenderInFlightImageDestroyIndex * 2], nullptr);
							vkDestroyImage(vk_hDevice, vk_ahDepthStencilImages[u8RenderInFlightImageDestroyIndex * 2], nullptr);
						}
						return false;
					}
					rDepthStencilImageLayoutTransitionTask.paFunctions[0] = [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
						const VkImageMemoryBarrier2 vk_aDepthStencilImageBarriers[] = {
							{
								.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
								.srcStageMask = VK_PIPELINE_STAGE_2_NONE,
								.srcAccessMask = VK_ACCESS_2_NONE,
								.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
								.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
								.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
								.newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
								.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
								.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
								.image = vk_ahDepthStencilImages[0],
								.subresourceRange = {
									.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
									.levelCount = 1,
									.layerCount = 1
								}
							}, {
								.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
								.srcStageMask = VK_PIPELINE_STAGE_2_NONE,
								.srcAccessMask = VK_ACCESS_2_NONE,
								.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
								.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
								.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
								.newLayout = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL,
								.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
								.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
								.image = vk_ahDepthStencilImages[1],
								.subresourceRange = {
									.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT,
									.levelCount = 1,
									.layerCount = 1
								}
							}, {
								.sType = vk_aDepthStencilImageBarriers[0].sType,
								.srcStageMask = vk_aDepthStencilImageBarriers[0].srcStageMask,
								.srcAccessMask = vk_aDepthStencilImageBarriers[0].srcAccessMask,
								.dstStageMask = vk_aDepthStencilImageBarriers[0].dstStageMask,
								.dstAccessMask = vk_aDepthStencilImageBarriers[0].dstAccessMask,
								.oldLayout = vk_aDepthStencilImageBarriers[0].oldLayout,
								.newLayout = vk_aDepthStencilImageBarriers[0].newLayout,
								.srcQueueFamilyIndex = vk_aDepthStencilImageBarriers[0].srcQueueFamilyIndex,
								.dstQueueFamilyIndex = vk_aDepthStencilImageBarriers[0].dstQueueFamilyIndex,
								.image = vk_ahDepthStencilImages[2],
								.subresourceRange = {
									.aspectMask = vk_aDepthStencilImageBarriers[0].subresourceRange.aspectMask,
									.levelCount = vk_aDepthStencilImageBarriers[0].subresourceRange.levelCount,
									.layerCount = vk_aDepthStencilImageBarriers[0].subresourceRange.layerCount
								}
							}, {
								.sType = vk_aDepthStencilImageBarriers[1].sType,
								.srcStageMask = vk_aDepthStencilImageBarriers[1].srcStageMask,
								.srcAccessMask = vk_aDepthStencilImageBarriers[1].srcAccessMask,
								.dstStageMask = vk_aDepthStencilImageBarriers[1].dstStageMask,
								.dstAccessMask = vk_aDepthStencilImageBarriers[1].dstAccessMask,
								.oldLayout = vk_aDepthStencilImageBarriers[1].oldLayout,
								.newLayout = vk_aDepthStencilImageBarriers[1].newLayout,
								.srcQueueFamilyIndex = vk_aDepthStencilImageBarriers[1].srcQueueFamilyIndex,
								.dstQueueFamilyIndex = vk_aDepthStencilImageBarriers[1].dstQueueFamilyIndex,
								.image = vk_ahDepthStencilImages[3],
								.subresourceRange = {
									.aspectMask = vk_aDepthStencilImageBarriers[1].subresourceRange.aspectMask,
									.levelCount = vk_aDepthStencilImageBarriers[1].subresourceRange.levelCount,
									.layerCount = vk_aDepthStencilImageBarriers[1].subresourceRange.layerCount
								}
							}
						};
						const VkDependencyInfo vk_imageTransitionBarrier = {
							.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
							.imageMemoryBarrierCount = sizeof(vk_aDepthStencilImageBarriers) / sizeof(vk_aDepthStencilImageBarriers[0]),
							.pImageMemoryBarriers = vk_aDepthStencilImageBarriers
						};
						vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_imageTransitionBarrier);
					};
					PUSH_TO_CALLSTACKTRACE(rDepthStencilImageLayoutTransitionTask.record(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT));
					PUSH_TO_CALLSTACKTRACE(rDepthStencilImageLayoutTransitionTask.submit(nullptr, 0, nullptr, nullptr, 0, nullptr, vk_hDepthStencilImageLayoutTransitionFence));
					return true;
				} else {
					RE_NOTE("The currently selected format for separated depth-stencil images isn't supported. Searching for combined format and using it instead");
					bool bAlternativeFormatFound = false;
					for (uint8_t u8FormatIndex = 0; u8FormatIndex < availableDepthStencilFormats.size(); u8FormatIndex++)
						if (!are_depth_stencil_images_separated(availableDepthStencilFormats[u8FormatIndex])) {
							u8IndexToSelectedDepthStencilFormat = u8FormatIndex;
							vk_eSelectedDepthFormat = availableDepthStencilFormats[u8IndexToSelectedDepthStencilFormat];
							bAlternativeFormatFound = true;
							break;
						}
					if (!bAlternativeFormatFound) [[unlikely]] {
						RE_FATAL_ERROR("Failed to find a depth-stencil-combined image format. There are no other options to gurantee depth and stencils");
						return false;
					}
				}
			}
			uint8_t u8DepthStencilImageCreateIndex = 0;
			while (u8DepthStencilImageCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
				if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_image(0, VK_IMAGE_TYPE_2D, vk_eSelectedDepthFormat, vk_depthStencilImageExtent, 1, 1, vk_eMsaaCount, vk_eDepthImageTiling, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 1, nullptr, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_ahDepthStencilImages[u8DepthStencilImageCreateIndex], &vk_ahDepthStencilImageMemories[u8DepthStencilImageCreateIndex]), bool)) {
					if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_image_view(vk_ahDepthStencilImages[u8DepthStencilImageCreateIndex], VK_IMAGE_VIEW_TYPE_2D, vk_eSelectedDepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, 0, 1, &vk_ahDepthStencilImageViews[u8DepthStencilImageCreateIndex]), bool)) {
						u8DepthStencilImageCreateIndex++;
						continue;
					} else
						RE_FATAL_ERROR("Failed to create Vulkan image view for depth-stencil-combined image at index ", u8DepthStencilImageCreateIndex);
					vkFreeMemory(vk_hDevice, vk_ahDepthStencilImageMemories[u8DepthStencilImageCreateIndex], nullptr);
					vkDestroyImage(vk_hDevice, vk_ahDepthStencilImages[u8DepthStencilImageCreateIndex], nullptr);
				} else
					RE_FATAL_ERROR("Failed to create Vulkan depth-stencil-combined image at index ", u8DepthStencilImageCreateIndex);
				break;
			}
			if (u8DepthStencilImageCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
				for (uint8_t u8DepthStencilImageDestroyIndex = 0; u8DepthStencilImageDestroyIndex < u8DepthStencilImageCreateIndex; u8DepthStencilImageDestroyIndex++) {
					vkDestroyImageView(vk_hDevice, vk_ahDepthStencilImageViews[u8DepthStencilImageDestroyIndex], nullptr);
					vkFreeMemory(vk_hDevice, vk_ahDepthStencilImageMemories[u8DepthStencilImageDestroyIndex], nullptr);
					vkDestroyImage(vk_hDevice, vk_ahDepthStencilImages[u8DepthStencilImageDestroyIndex], nullptr);
				}
				return false;
			}
			rDepthStencilImageLayoutTransitionTask.paFunctions[0] = [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
				const VkImageMemoryBarrier2 vk_aDepthStencilImageBarriers[] = {
					{
						.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
						.srcStageMask = VK_PIPELINE_STAGE_2_NONE,
						.srcAccessMask = VK_ACCESS_2_NONE,
						.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
						.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
						.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
						.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
						.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
						.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
						.image = vk_ahDepthStencilImages[0],
						.subresourceRange = {
							.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
							.levelCount = 1,
							.layerCount = 1
						}
					}, {
						.sType = vk_aDepthStencilImageBarriers[0].sType,
						.srcStageMask = vk_aDepthStencilImageBarriers[0].srcStageMask,
						.srcAccessMask = vk_aDepthStencilImageBarriers[0].srcAccessMask,
						.dstStageMask = vk_aDepthStencilImageBarriers[0].dstStageMask,
						.dstAccessMask = vk_aDepthStencilImageBarriers[0].dstAccessMask,
						.oldLayout = vk_aDepthStencilImageBarriers[0].oldLayout,
						.newLayout = vk_aDepthStencilImageBarriers[0].newLayout,
						.srcQueueFamilyIndex = vk_aDepthStencilImageBarriers[0].srcQueueFamilyIndex,
						.dstQueueFamilyIndex = vk_aDepthStencilImageBarriers[0].dstQueueFamilyIndex,
						.image = vk_ahDepthStencilImages[1],
						.subresourceRange = {
							.aspectMask = vk_aDepthStencilImageBarriers[0].subresourceRange.aspectMask,
							.levelCount = vk_aDepthStencilImageBarriers[0].subresourceRange.levelCount,
							.layerCount = vk_aDepthStencilImageBarriers[0].subresourceRange.layerCount
						}
					}
				};
				const VkDependencyInfo vk_imageTransitionBarrier = {
					.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
					.imageMemoryBarrierCount = sizeof(vk_aDepthStencilImageBarriers) / sizeof(vk_aDepthStencilImageBarriers[0]),
					.pImageMemoryBarriers = vk_aDepthStencilImageBarriers
				};
				vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_imageTransitionBarrier);
			};
		} else [[likely]] {
			constexpr VkFormat vk_aeDepthOnlyFormats[] = {
				VK_FORMAT_D32_SFLOAT,
				VK_FORMAT_X8_D24_UNORM_PACK32,
				VK_FORMAT_D16_UNORM
			};
			uint8_t u8SelectedDepthOnlyFormatIndex;
			for (uint8_t u8DepthOnlyFormatIndex = 0; u8DepthOnlyFormatIndex < sizeof(vk_aeDepthOnlyFormats) / sizeof(vk_aeDepthOnlyFormats[0]); u8DepthOnlyFormatIndex++)
				if (std::find(availableDepthStencilFormats.begin(), availableDepthStencilFormats.end(), vk_aeDepthOnlyFormats[u8DepthOnlyFormatIndex]) != availableDepthStencilFormats.end()) {
					u8SelectedDepthOnlyFormatIndex = u8DepthOnlyFormatIndex;
					break;
				}
			for (uint8_t u8AvailableFormatIndex = 0; u8AvailableFormatIndex < availableDepthStencilFormats.size(); u8AvailableFormatIndex++)
				if (vk_aeDepthOnlyFormats[u8SelectedDepthOnlyFormatIndex] == availableDepthStencilFormats[u8AvailableFormatIndex]) {
					u8IndexToSelectedDepthStencilFormat = u8AvailableFormatIndex;
					break;
				}
			uint8_t u8DepthImageCreateIndex = 0;
			while (u8DepthImageCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
				if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_image(0, VK_IMAGE_TYPE_2D, vk_aeDepthOnlyFormats[u8SelectedDepthOnlyFormatIndex], vk_depthStencilImageExtent, 1, 1, vk_eMsaaCount, vk_eDepthImageTiling, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 1, nullptr, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_ahDepthStencilImages[u8DepthImageCreateIndex], &vk_ahDepthStencilImageMemories[u8DepthImageCreateIndex]), bool)) {
					if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_image_view(vk_ahDepthStencilImages[u8DepthImageCreateIndex], VK_IMAGE_VIEW_TYPE_2D, vk_aeDepthOnlyFormats[u8SelectedDepthOnlyFormatIndex], VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1, &vk_ahDepthStencilImageViews[u8DepthImageCreateIndex]), bool)) {
						u8DepthImageCreateIndex++;
						continue;
					} else
						RE_FATAL_ERROR("Failed to create Vulkan image view for depth image at index ", u8DepthImageCreateIndex);
					vkFreeMemory(vk_hDevice, vk_ahDepthStencilImageMemories[u8DepthImageCreateIndex], nullptr);
					vkDestroyImage(vk_hDevice, vk_ahDepthStencilImages[u8DepthImageCreateIndex], nullptr);
				} else
					RE_FATAL_ERROR("Failed to create Vulkan depth image at index ", u8DepthImageCreateIndex);
				break;
			}
			if (u8DepthImageCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
				for (uint8_t u8DepthImageDestroyIndex = 0; u8DepthImageDestroyIndex < u8DepthImageCreateIndex; u8DepthImageDestroyIndex++) {
					vkDestroyImageView(vk_hDevice, vk_ahDepthStencilImageViews[u8DepthImageDestroyIndex], nullptr);
					vkFreeMemory(vk_hDevice, vk_ahDepthStencilImageMemories[u8DepthImageDestroyIndex], nullptr);
					vkDestroyImage(vk_hDevice, vk_ahDepthStencilImages[u8DepthImageDestroyIndex], nullptr);
				}
				return false;
			}
			rDepthStencilImageLayoutTransitionTask.paFunctions[0] = [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
				const VkImageMemoryBarrier2 vk_aDepthImageBarriers[] = {
					{
						.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
						.srcStageMask = VK_PIPELINE_STAGE_2_NONE,
						.srcAccessMask = VK_ACCESS_2_NONE,
						.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
						.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
						.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
						.newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
						.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
						.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
						.image = vk_ahDepthStencilImages[0],
						.subresourceRange = {
							.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
							.levelCount = 1,
							.layerCount = 1
						}
					}, {
						.sType = vk_aDepthImageBarriers[0].sType,
						.srcStageMask = vk_aDepthImageBarriers[0].srcStageMask,
						.srcAccessMask = vk_aDepthImageBarriers[0].srcAccessMask,
						.dstStageMask = vk_aDepthImageBarriers[0].dstStageMask,
						.dstAccessMask = vk_aDepthImageBarriers[0].dstAccessMask,
						.oldLayout = vk_aDepthImageBarriers[0].oldLayout,
						.newLayout = vk_aDepthImageBarriers[0].newLayout,
						.srcQueueFamilyIndex = vk_aDepthImageBarriers[0].srcQueueFamilyIndex,
						.dstQueueFamilyIndex = vk_aDepthImageBarriers[0].dstQueueFamilyIndex,
						.image = vk_ahDepthStencilImages[1],
						.subresourceRange = {
							.aspectMask = vk_aDepthImageBarriers[0].subresourceRange.aspectMask,
							.levelCount = vk_aDepthImageBarriers[0].subresourceRange.levelCount,
							.layerCount = vk_aDepthImageBarriers[0].subresourceRange.layerCount
						}
					}
				};
				const VkDependencyInfo vk_imageTransitionBarrier = {
					.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
					.imageMemoryBarrierCount = sizeof(vk_aDepthImageBarriers) / sizeof(vk_aDepthImageBarriers[0]),
					.pImageMemoryBarriers = vk_aDepthImageBarriers
				};
				vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_imageTransitionBarrier);
			};
		}
		PUSH_TO_CALLSTACKTRACE(rDepthStencilImageLayoutTransitionTask.record(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT));
		PUSH_TO_CALLSTACKTRACE(rDepthStencilImageLayoutTransitionTask.submit(nullptr, 0, nullptr, nullptr, 0, nullptr, vk_hDepthStencilImageLayoutTransitionFence));
		return true;
	}

	void destroy_depth_stencil_buffers() {
		const uint8_t u8ImageCount = vk_ahDepthStencilImages.size() / ((bStencilsEnabled && are_depth_stencil_images_separated(availableDepthStencilFormats[u8IndexToSelectedDepthStencilFormat])) ? 1 : 2);
		for (uint8_t u8ImageIndex = 0; u8ImageIndex < u8ImageCount; u8ImageIndex++) {
			vkDestroyImageView(vk_hDevice, vk_ahDepthStencilImageViews[u8ImageIndex], nullptr);
			vkFreeMemory(vk_hDevice, vk_ahDepthStencilImageMemories[u8ImageIndex], nullptr);
			vkDestroyImage(vk_hDevice, vk_ahDepthStencilImages[u8ImageIndex], nullptr);
		}
	}

	void enable_stencil_buffer(const bool bEnable) {
		RE_NOTE("Stencil buffers are currently unused and the engine doesn't provide any use cases for them. They will be kept disabled to save memory");
	}

	bool are_stencil_buffers_enabled() {
		return bStencilsEnabled;
	}

}
