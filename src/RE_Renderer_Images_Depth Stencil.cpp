#include "RE_Renderer_Images_Internal.hpp"
#include "RE_Window.hpp"
#include "RE_Main.hpp"

namespace RE {
	
	VkImage vk_hDepthImage = VK_NULL_HANDLE,
		vk_hStencilImage = VK_NULL_HANDLE;
	std::array<VkImageView, RE_VK_FRAMES_IN_FLIGHT> depthImageViews,
		stencilImageViews;
	VkFormat vk_eSelectedDepthFormat,
		vk_eStelectedStencilFormat;
	VkImageLayout vk_eDepthLayout,
		vk_eStencilLayout;
	static DepthPrecission eDepthPrecission = RE_DEPTH_PRECISSION_LOW;
	uint8_t m8DepthStencilFeatures = 0;
	static bool bStencilsEnabled = false;

	static void adjust_depth_stencil_mode(DepthPrecission &reDepthPrecission, bool &rbStencilsEnabled) {
		PRINT_DEBUG("Adjusting depth precission and enabled stencils to supported features and formats");
		const bool bStencilsRequired = rbStencilsEnabled;
		int8_t i8NextDepthPrecission;
		for (uint8_t u8StencilIter = 0; u8StencilIter < 2; u8StencilIter++) {
			if (!bStencilsRequired || rbStencilsEnabled)
				for (int8_t i8DepthIter = 0; i8DepthIter < 3; i8DepthIter++) {
					i8NextDepthPrecission = static_cast<int8_t>(reDepthPrecission) + i8DepthIter;
					if (i8NextDepthPrecission >= 3)
						i8NextDepthPrecission = static_cast<int8_t>(reDepthPrecission) - (i8DepthIter - (2 - static_cast<int8_t>(reDepthPrecission)));
					if (rbStencilsEnabled) {
						PRINT_DEBUG("Checking if depth precission ", std::hex, i8NextDepthPrecission, " with stencils combined is supported");
						switch (i8NextDepthPrecission) {
							case RE_DEPTH_PRECISSION_LOW:
								if ((m8DepthStencilFeatures & DEPTH_STENCIL_FEATURE_D16_S8_SUPPORTED_BIT))
									goto COMPATIBLE_DEPTH_STENCIL_MODE_FOUND;
								break;
							case RE_DEPTH_PRECISSION_MODERATE:
								if ((m8DepthStencilFeatures & DEPTH_STENCIL_FEATURE_D24_S8_SUPPORTED_BIT))
									goto COMPATIBLE_DEPTH_STENCIL_MODE_FOUND;
								break;
							case RE_DEPTH_PRECISSION_HIGH:
								if ((m8DepthStencilFeatures & DEPTH_STENCIL_FEATURE_D32_S8_SUPPORTED_BIT))
									goto COMPATIBLE_DEPTH_STENCIL_MODE_FOUND;
								break;
						}
						if ((m8DepthStencilFeatures & DEPTH_STENCIL_FEATURE_S8_SUPPORTED_BIT)) {
							PRINT_DEBUG("Checking if depth precission ", std::hex, i8NextDepthPrecission, " with separated stencils are supported");
							switch (i8NextDepthPrecission) {
								case RE_DEPTH_PRECISSION_LOW:
									if ((m8DepthStencilFeatures & DEPTH_STENCIL_FEATURE_D16_SUPPORTED_BIT))
										goto COMPATIBLE_DEPTH_STENCIL_MODE_FOUND;
									break;
								case RE_DEPTH_PRECISSION_MODERATE:
									if ((m8DepthStencilFeatures & DEPTH_STENCIL_FEATURE_D24_SUPPORTED_BIT))
										goto COMPATIBLE_DEPTH_STENCIL_MODE_FOUND;
									break;
								case RE_DEPTH_PRECISSION_HIGH:
									if ((m8DepthStencilFeatures & DEPTH_STENCIL_FEATURE_D32_SUPPORTED_BIT))
										goto COMPATIBLE_DEPTH_STENCIL_MODE_FOUND;
									break;
							}
						}
					} else {
						PRINT_DEBUG("Checking if depth precission ", std::hex, i8NextDepthPrecission, " without any stencils are supported");
						switch (i8NextDepthPrecission) {
							case RE_DEPTH_PRECISSION_LOW:
								if ((m8DepthStencilFeatures & DEPTH_STENCIL_FEATURE_D16_SUPPORTED_BIT))
									goto COMPATIBLE_DEPTH_STENCIL_MODE_FOUND;
								break;
							case RE_DEPTH_PRECISSION_MODERATE:
								if ((m8DepthStencilFeatures & DEPTH_STENCIL_FEATURE_D24_SUPPORTED_BIT))
									goto COMPATIBLE_DEPTH_STENCIL_MODE_FOUND;
								break;
							case RE_DEPTH_PRECISSION_HIGH:
								if ((m8DepthStencilFeatures & DEPTH_STENCIL_FEATURE_D32_SUPPORTED_BIT))
									goto COMPATIBLE_DEPTH_STENCIL_MODE_FOUND;
								break;
						}
					}
				}
			rbStencilsEnabled = !rbStencilsEnabled;
		}
		RE_ABORT("No suitable depth precission found. Depth precission was ", std::hex, reDepthPrecission, ", stencils enabled was ", std::dec, rbStencilsEnabled, " and feature flags were set to ", std::hex, m8DepthStencilFeatures);

	COMPATIBLE_DEPTH_STENCIL_MODE_FOUND:
		PRINT_DEBUG("Writing adjusted depth precission");
		reDepthPrecission = static_cast<DepthPrecission>(i8NextDepthPrecission);
		if (reDepthPrecission == RE_DEPTH_PRECISSION_MODERATE && !bStencilsEnabled && (m8DepthStencilFeatures & DEPTH_STENCIL_FEATURE_D32_SUPPORTED_BIT)) {
			PRINT_DEBUG("Depth precission will be upgraded from moderate to high quality without stencils (D24 -> D32)");
			reDepthPrecission = RE_DEPTH_PRECISSION_HIGH;
		} else if (reDepthPrecission == RE_DEPTH_PRECISSION_LOW && bStencilsEnabled && (m8DepthStencilFeatures & DEPTH_STENCIL_FEATURE_D24_S8_SUPPORTED_BIT)) {
			PRINT_DEBUG("Depth precission will be upgraded from low to moderate quality without stencils (D16_S8 -> D24_S8)");
			reDepthPrecission = RE_DEPTH_PRECISSION_MODERATE;
		}
	}

	void find_suitable_depth_stencil_formats() {
		PRINT_DEBUG("Setting data structures up for checking availability of Vulkan formats");
		VkPhysicalDeviceImageFormatInfo2 vk_imageFormatInfo;
		vk_imageFormatInfo.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2;
		vk_imageFormatInfo.pNext = nullptr;
		vk_imageFormatInfo.type = VK_IMAGE_TYPE_2D;
		vk_imageFormatInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		vk_imageFormatInfo.flags = 0;
		VkImageFormatProperties2 vk_imageFormatProperties2;
		vk_imageFormatProperties2.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2;
		vk_imageFormatProperties2.pNext = nullptr;
		const VkImageFormatProperties &vk_rImageFormatProperties = vk_imageFormatProperties2.imageFormatProperties;
		bool bDepthSupported, bStencilsSupported;
		constexpr VkFormat vk_aeDepthStencilFormats[] = {
			VK_FORMAT_S8_UINT,
			VK_FORMAT_D16_UNORM,
			VK_FORMAT_X8_D24_UNORM_PACK32,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D32_SFLOAT_S8_UINT
		};
		for (uint8_t u8TransientAttachmentIter = 0; u8TransientAttachmentIter < 2; u8TransientAttachmentIter++) {
			switch (u8TransientAttachmentIter) {
				case 0:
					vk_imageFormatInfo.usage = vk_mDepthStencilImageUsages | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
					break;
				case 1:
					vk_imageFormatInfo.usage = vk_mDepthStencilImageUsages;
					break;
				[[unlikely]] default:
					RE_ABORT("The iteration index on image usage flags for depth and stencil images is unknown: ", std::hex, u8TransientAttachmentIter);
			}
			m8DepthStencilFeatures = 0;
			bDepthSupported = false;
			bStencilsSupported = false;
			for (const VkFormat vk_eDepthStencilFormat : vk_aeDepthStencilFormats) {
				PRINT_DEBUG("Checking availability of Vulkan format ", std::hex, vk_eDepthStencilFormat, " for depth and stencil");
				vk_imageFormatInfo.format = vk_eDepthStencilFormat;
				const VkResult vk_eResult = vkGetPhysicalDeviceImageFormatProperties2(SELECTED_PHYSICAL_VULKAN_DEVICE, &vk_imageFormatInfo, &vk_imageFormatProperties2);
				switch (vk_eResult) {
					case VK_SUCCESS:
						if (vk_rImageFormatProperties.maxExtent.width >= largestMonitorSize[0]
								&& vk_rImageFormatProperties.maxExtent.height >= largestMonitorSize[1]
								&& vk_rImageFormatProperties.maxArrayLayers >= RE_VK_FRAMES_IN_FLIGHT) {
							PRINT_DEBUG("Vulkan format ", std::hex, vk_eDepthStencilFormat, " is supported allowing to occupy ", std::dec, vk_rImageFormatProperties.maxResourceSize, " bytes, MSAA modes ", std::hex, vk_rImageFormatProperties.sampleCounts, " and maximum size (", std::dec, vk_rImageFormatProperties.maxExtent.width, ", ", vk_rImageFormatProperties.maxExtent.height, "). Adding to flags of supported depth-stencil formats");
							switch (vk_eDepthStencilFormat) {
								case VK_FORMAT_S8_UINT:
									m8DepthStencilFeatures |= DEPTH_STENCIL_FEATURE_S8_SUPPORTED_BIT;
									bStencilsSupported = true;
									break;
								case VK_FORMAT_D16_UNORM:
									m8DepthStencilFeatures |= DEPTH_STENCIL_FEATURE_D16_SUPPORTED_BIT;
									bDepthSupported = true;
									break;
								case VK_FORMAT_X8_D24_UNORM_PACK32:
									m8DepthStencilFeatures |= DEPTH_STENCIL_FEATURE_D24_SUPPORTED_BIT;
									bDepthSupported = true;
									break;
								case VK_FORMAT_D32_SFLOAT:
									m8DepthStencilFeatures |= DEPTH_STENCIL_FEATURE_D32_SUPPORTED_BIT;
									bDepthSupported = true;
									break;
								case VK_FORMAT_D16_UNORM_S8_UINT:
									m8DepthStencilFeatures |= DEPTH_STENCIL_FEATURE_D16_S8_SUPPORTED_BIT;
									bDepthSupported = true;
									bStencilsSupported = true;
									break;
								case VK_FORMAT_D24_UNORM_S8_UINT:
									m8DepthStencilFeatures |= DEPTH_STENCIL_FEATURE_D24_S8_SUPPORTED_BIT;
									bDepthSupported = true;
									bStencilsSupported = true;
									break;
								case VK_FORMAT_D32_SFLOAT_S8_UINT:
									m8DepthStencilFeatures |= DEPTH_STENCIL_FEATURE_D32_S8_SUPPORTED_BIT;
									bDepthSupported = true;
									bStencilsSupported = true;
									break;
								[[unlikely]] default:
									RE_ABORT("Unknown Vulkan format ", std::hex, vk_eDepthStencilFormat, " used to determine support for depth and stencil");
							}
						}
						break;
					case VK_ERROR_FORMAT_NOT_SUPPORTED:
						break;
					default:
						RE_ERROR("Failed to retrieve image properties of depth-stencil format ", std::hex, vk_eDepthStencilFormat, " (Vulkan error code: ", vk_eResult, ")");
				}
			}
			if (bDepthSupported && bStencilsSupported) {
				if ((vk_imageFormatInfo.usage & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)) {
					PRINT_DEBUG("Depth and stencil images can be used as transient attachments");
					m8DepthStencilFeatures |= DEPTH_STENCIL_FEATURE_TRANSIENT_ATTACHMENT_BIT;
				}
				break;
			}
		}
		if (!bDepthSupported || !bStencilsSupported) [[unlikely]]
			RE_ABORT("There aren't any depth or stencil formats supported");
		PRINT_DEBUG("Checking availability of currently selected depth-stencil format");
		adjust_depth_stencil_mode(eDepthPrecission, bStencilsEnabled);
	}

	bool create_depth_stencil_image() {
		PRINT_DEBUG("Setting data structure up for creating depth-stencil images");
		VkImageCreateInfo vk_depthStencilCreateInfo;
		vk_depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		vk_depthStencilCreateInfo.pNext = nullptr;
		vk_depthStencilCreateInfo.flags = 0;
		vk_depthStencilCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		vk_depthStencilCreateInfo.extent.width = renderImageSize[0];
		vk_depthStencilCreateInfo.extent.height = renderImageSize[1];
		vk_depthStencilCreateInfo.extent.depth = 1;
		vk_depthStencilCreateInfo.mipLevels = 1;
		vk_depthStencilCreateInfo.arrayLayers = RE_VK_FRAMES_IN_FLIGHT;
		vk_depthStencilCreateInfo.samples = vk_eMsaaCount;
		vk_depthStencilCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		vk_depthStencilCreateInfo.usage = vk_mDepthStencilImageUsages;
		if ((m8DepthStencilFeatures & DEPTH_STENCIL_FEATURE_TRANSIENT_ATTACHMENT_BIT))
			vk_depthStencilCreateInfo.usage |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
		vk_depthStencilCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		vk_depthStencilCreateInfo.queueFamilyIndexCount = 1;
		vk_depthStencilCreateInfo.pQueueFamilyIndices = nullptr;
		vk_depthStencilCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkResult vk_eResult;
		if (bStencilsEnabled) {
			if ((m8DepthStencilFeatures & (DEPTH_STENCIL_FEATURE_D16_S8_SUPPORTED_BIT | DEPTH_STENCIL_FEATURE_D24_S8_SUPPORTED_BIT | DEPTH_STENCIL_FEATURE_D32_S8_SUPPORTED_BIT))) {
				switch (eDepthPrecission) {
					case RE_DEPTH_PRECISSION_LOW:
						vk_depthStencilCreateInfo.format = VK_FORMAT_D16_UNORM_S8_UINT;
						break;
					case RE_DEPTH_PRECISSION_MODERATE:
						vk_depthStencilCreateInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
						break;
					case RE_DEPTH_PRECISSION_HIGH:
						vk_depthStencilCreateInfo.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
						break;
					[[unlikely]] default:
						RE_ABORT("Unknown depth precission ", std::hex, eDepthPrecission);
				}
				PRINT_DEBUG("Creating Vulkan image with depth and stencil combined format ", std::hex, vk_depthStencilCreateInfo.format);
				vk_eResult = vkCreateImage(vk_hDevice, &vk_depthStencilCreateInfo, nullptr, &vk_hDepthImage);
				if (vk_eResult == VK_SUCCESS) {
					PRINT_DEBUG("Saving Vulkan format and image layouts for combined depth-stencil image");
					vk_hStencilImage = vk_hDepthImage;
					vk_eSelectedDepthFormat = vk_depthStencilCreateInfo.format;
					vk_eStelectedStencilFormat = vk_eSelectedDepthFormat;
					vk_eDepthLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
					vk_eStencilLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
					return true;
				} else
					RE_FATAL_ERROR("Failed to create Vulkan image with depth and stencil combined format ", std::hex, vk_depthStencilCreateInfo.format, " (Vulkan error code: ", vk_eResult, ")");
			} else {
				PRINT_DEBUG("Creating Vulkan image with stencil format only");
				vk_depthStencilCreateInfo.format = VK_FORMAT_S8_UINT;
				vk_eResult = vkCreateImage(vk_hDevice, &vk_depthStencilCreateInfo, nullptr, &vk_hStencilImage);
				if (vk_eResult == VK_SUCCESS) {
					switch (eDepthPrecission) {
						case RE_DEPTH_PRECISSION_LOW:
							vk_depthStencilCreateInfo.format = VK_FORMAT_D16_UNORM;
							break;
						case RE_DEPTH_PRECISSION_MODERATE:
							vk_depthStencilCreateInfo.format = VK_FORMAT_X8_D24_UNORM_PACK32;
							break;
						case RE_DEPTH_PRECISSION_HIGH:
							vk_depthStencilCreateInfo.format = VK_FORMAT_D32_SFLOAT;
							break;
						[[unlikely]] default:
							RE_ABORT("Unknown depth precission ", std::hex, eDepthPrecission);
					}
					PRINT_DEBUG("Creating Vulkan image with depth format ", std::hex, vk_depthStencilCreateInfo.format);
					vk_eResult = vkCreateImage(vk_hDevice, &vk_depthStencilCreateInfo, nullptr, &vk_hDepthImage);
					if (vk_eResult == VK_SUCCESS) {
						PRINT_DEBUG("Saving Vulkan format and image layouts for separate depth and stencil images");
						vk_eSelectedDepthFormat = vk_depthStencilCreateInfo.format;
						vk_eStelectedStencilFormat = VK_FORMAT_S8_UINT;
						vk_eDepthLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
						vk_eStencilLayout = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
						return true;
					} else
						RE_FATAL_ERROR("Failed to create separate Vulkan image for depth data (Vulkan error code: ", std::hex, vk_eResult, ")");
					PRINT_DEBUG("Destroying Vulkan image ", vk_hStencilImage, " storing stencils due to failure ");
					vkDestroyImage(vk_hDevice, vk_hStencilImage, nullptr);
				} else
					RE_FATAL_ERROR("Failed to create separate Vulkan image for stencil data (Vulkan error code: ", std::hex, vk_eResult, ")");
			}
		} else {
			switch (eDepthPrecission) {
				case RE_DEPTH_PRECISSION_LOW:
					vk_depthStencilCreateInfo.format = VK_FORMAT_D16_UNORM;
					break;
				case RE_DEPTH_PRECISSION_MODERATE:
					vk_depthStencilCreateInfo.format = VK_FORMAT_X8_D24_UNORM_PACK32;
					break;
				case RE_DEPTH_PRECISSION_HIGH:
					vk_depthStencilCreateInfo.format = VK_FORMAT_D32_SFLOAT;
					break;
				[[unlikely]] default:
					RE_ABORT("Unknown depth precission ", std::hex, eDepthPrecission);
			}
			PRINT_DEBUG("Creating Vulkan image with depth-only format ", std::hex, vk_depthStencilCreateInfo.format);
			vk_eResult = vkCreateImage(vk_hDevice, &vk_depthStencilCreateInfo, nullptr, &vk_hDepthImage);
			if (vk_eResult == VK_SUCCESS) {
				PRINT_DEBUG("Saving Vulkan format and image layouts for depth-only image");
				vk_hStencilImage = VK_NULL_HANDLE;
				vk_eSelectedDepthFormat = vk_depthStencilCreateInfo.format;
				vk_eStelectedStencilFormat = VK_FORMAT_UNDEFINED;
				vk_eDepthLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
				vk_eStencilLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				return true;
			} else
				RE_FATAL_ERROR("Failed to create Vulkan image with depth-only format ", std::hex, vk_depthStencilCreateInfo.format);
		}
		vk_hDepthImage = VK_NULL_HANDLE;
		vk_hStencilImage = VK_NULL_HANDLE;
		return false;
	}

	bool create_depth_stencil_image_views() {
		PRINT_DEBUG("Setting data structures up for creating Vulkan image views referring depth-stencil data");
		VkImageViewCreateInfo vk_depthStencilViewCreateInfo;
		vk_depthStencilViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		vk_depthStencilViewCreateInfo.pNext = nullptr;
		vk_depthStencilViewCreateInfo.flags = 0;
		vk_depthStencilViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		vk_depthStencilViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		vk_depthStencilViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		vk_depthStencilViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		vk_depthStencilViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		vk_depthStencilViewCreateInfo.subresourceRange.baseMipLevel = 0;
		vk_depthStencilViewCreateInfo.subresourceRange.levelCount = 1;
		vk_depthStencilViewCreateInfo.subresourceRange.layerCount = 1;
		VkResult vk_eResult;
		uint8_t u8FrameInFlightCreateIndex = 0;
		while (u8FrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
			PRINT_DEBUG("Creating Vulkan image views at frame-in-flight index ", u8FrameInFlightCreateIndex);
			vk_depthStencilViewCreateInfo.image = vk_hDepthImage;
			vk_depthStencilViewCreateInfo.format = vk_eSelectedDepthFormat;
			vk_depthStencilViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			vk_depthStencilViewCreateInfo.subresourceRange.baseArrayLayer = u8FrameInFlightCreateIndex;
			if (bStencilsEnabled) {
				if (vk_hDepthImage != vk_hStencilImage) {
					PRINT_DEBUG("Creating Vulkan image view pointing at depth image ", vk_depthStencilViewCreateInfo.image, " with format ", std::hex, vk_depthStencilViewCreateInfo.format);
					vk_eResult = vkCreateImageView(vk_hDevice, &vk_depthStencilViewCreateInfo, nullptr, &depthImageViews[u8FrameInFlightCreateIndex]);
					if (vk_eResult == VK_SUCCESS) {
						vk_depthStencilViewCreateInfo.image = vk_hStencilImage;
						vk_depthStencilViewCreateInfo.format = vk_eStelectedStencilFormat;
						vk_depthStencilViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
						PRINT_DEBUG("Creating Vulkan image view pointing at stencil image ", vk_depthStencilViewCreateInfo.image, " with format ", std::hex, vk_depthStencilViewCreateInfo.format);
						vk_eResult = vkCreateImageView(vk_hDevice, &vk_depthStencilViewCreateInfo, nullptr, &stencilImageViews[u8FrameInFlightCreateIndex]);
						if (vk_eResult == VK_SUCCESS) {
							u8FrameInFlightCreateIndex++;
							continue;
						} else
							RE_FATAL_ERROR("Failed to create Vulkan image view at index ", u8FrameInFlightCreateIndex, " pointing at stencil image ", vk_depthStencilViewCreateInfo.image, " with format ", std::hex, vk_depthStencilViewCreateInfo.format, " (error code: ", vk_eResult, ")");
						PRINT_DEBUG("Destroying Vulkan image view pointing at depth data due to failure creating an image view to stencil data");
						vkDestroyImageView(vk_hDevice, depthImageViews[u8FrameInFlightCreateIndex], nullptr);
					} else
						RE_FATAL_ERROR("Failed to create Vulkan image view at index ", u8FrameInFlightCreateIndex, " pointing at depth image ", vk_depthStencilViewCreateInfo.image, " with format ", std::hex, vk_depthStencilViewCreateInfo.format, " (error code: ", vk_eResult, ")");
				} else {
					PRINT_DEBUG("Creating Vulkan image view pointing at depth-stencil-combined image ", vk_depthStencilViewCreateInfo.image, " with format ", std::hex, vk_depthStencilViewCreateInfo.format);
					vk_depthStencilViewCreateInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
					vk_eResult = vkCreateImageView(vk_hDevice, &vk_depthStencilViewCreateInfo, nullptr, &depthImageViews[u8FrameInFlightCreateIndex]);
					if (vk_eResult == VK_SUCCESS) {
						stencilImageViews[u8FrameInFlightCreateIndex] = depthImageViews[u8FrameInFlightCreateIndex];
						u8FrameInFlightCreateIndex++;
						continue;
					} else
						RE_FATAL_ERROR("Failed to create Vulkan image view at index ", u8FrameInFlightCreateIndex, " pointing at depth-stencil-combined image ", vk_depthStencilViewCreateInfo.image, " with format ", std::hex, vk_depthStencilViewCreateInfo.format, " (error code: ", vk_eResult, ")");
				}
			} else {
				PRINT_DEBUG("Creating Vulkan image view pointing at depth-only image ", vk_depthStencilViewCreateInfo.image, " with format ", std::hex, vk_depthStencilViewCreateInfo.format, " (error code: ", vk_eResult, ")");
				stencilImageViews[u8FrameInFlightCreateIndex] = VK_NULL_HANDLE;
				vk_eResult = vkCreateImageView(vk_hDevice, &vk_depthStencilViewCreateInfo, nullptr, &depthImageViews[u8FrameInFlightCreateIndex]);
				if (vk_eResult == VK_SUCCESS) {
					u8FrameInFlightCreateIndex++;
					continue;
				} else
					RE_FATAL_ERROR("Failed to create Vulkan image view at index ", u8FrameInFlightCreateIndex, " pointing at depth-only image ", vk_depthStencilViewCreateInfo.image, " with format ", std::hex, vk_depthStencilViewCreateInfo.format, " (error code: ", vk_eResult, ")");
			}
			break;
		}
		if (u8FrameInFlightCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
			return true;
		uint8_t u8FrameInFlightDestroyIndex = 0;
		while (u8FrameInFlightDestroyIndex < u8FrameInFlightCreateIndex) {
			PRINT_DEBUG("Destroying Vulkan image view at index ", u8FrameInFlightDestroyIndex, " pointing at depth data due to failure creating all image views pointing at depth and stencil images");
			vkDestroyImageView(vk_hDevice, depthImageViews[u8FrameInFlightDestroyIndex], nullptr);
			if (bStencilsEnabled && vk_hDepthImage != vk_hStencilImage) {
				PRINT_DEBUG("Destroying Vulkan image view at index ", u8FrameInFlightDestroyIndex, " pointing at stencil data due to failure creating all image views pointing at depth and stencil images");
				vkDestroyImageView(vk_hDevice, stencilImageViews[u8FrameInFlightDestroyIndex], nullptr);
			}
		}
		depthImageViews.fill(VK_NULL_HANDLE);
		stencilImageViews.fill(VK_NULL_HANDLE);
		return false;
	}

	void destroy_depth_stencil_image() {
		PRINT_DEBUG("Destroying Vulkan images referring depth data");
		vkDestroyImage(vk_hDevice, vk_hDepthImage, nullptr);
		if (bStencilsEnabled && vk_hDepthImage != vk_hStencilImage) {
			PRINT_DEBUG("Destroying Vulkan image storing stencil data");
			vkDestroyImage(vk_hDevice, vk_hStencilImage, nullptr);
		}
		vk_hDepthImage = VK_NULL_HANDLE;
		vk_hStencilImage = VK_NULL_HANDLE;
	}

	void destroy_depth_stencil_image_views() {
		PRINT_DEBUG("Destroying Vulkan image views referring depth data");
		for (const VkImageView vk_hDepthImageView : depthImageViews)
			vkDestroyImageView(vk_hDevice, vk_hDepthImageView, nullptr);
		if (bStencilsEnabled && vk_hDepthImage != vk_hStencilImage) {
			PRINT_DEBUG("Destroying Vulkan image views referring stencil data");
			for (const VkImageView vk_hStencilImageView : stencilImageViews)
				vkDestroyImageView(vk_hDevice, vk_hStencilImageView, nullptr);
		}
		depthImageViews.fill(VK_NULL_HANDLE);
		stencilImageViews.fill(VK_NULL_HANDLE);
	}

	void set_depth_stencil_settings(DepthPrecission eNewDepthPrecission, bool bNewStencilsEnabled) {
		PRINT_DEBUG("Adjusting depth precission");
		adjust_depth_stencil_mode(eNewDepthPrecission, bNewStencilsEnabled);
		if (eDepthPrecission == eNewDepthPrecission && bStencilsEnabled == bNewStencilsEnabled)
			return;
		if (bRunning) {
			PRINT_DEBUG("Destroying depth and stencil Vulkan images");
			wait_for_rendering_finished();
			destroy_images_renderer();
		}
		PRINT_DEBUG("Updating depth and stencil settings");
		eDepthPrecission = eNewDepthPrecission;
		bStencilsEnabled = bNewStencilsEnabled;
		if (bRunning) {
			PRINT_DEBUG("Recreating depth and stencil Vulkan images");
			create_images_renderer();
		}
	}

}
