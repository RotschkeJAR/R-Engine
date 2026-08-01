#include "RE_Renderer_Images_Internal.hpp"
#include "RE_Window.hpp"
#include "RE_Main.hpp"
#include "RE_Settings.hpp"

namespace RE {
	
	VkImage vk_hDepthStencilImage = VK_NULL_HANDLE;
	VkImageView vk_ahDepthStencilImageViews[RE_VK_FRAMES_IN_FLIGHT] = {};
	VkFormat vk_eSelectedDepthStencilFormat = VK_FORMAT_D16_UNORM;
	DepthStencilFeatureFlags mDepthStencilFeatures = 0;

	static DepthStencilFeatureBits get_depth_stencil_feature(DepthPrecission ePrecission, bool bStencils) {
		if (bStencils) {
			switch (ePrecission) {
				case RE_DEPTH_PRECISSION_LOW:
					return DEPTH_STENCIL_FEATURE_D16_S8_SUPPORTED_BIT;
				case RE_DEPTH_PRECISSION_MODERATE:
					return DEPTH_STENCIL_FEATURE_D24_S8_SUPPORTED_BIT;
				case RE_DEPTH_PRECISSION_HIGH:
					return DEPTH_STENCIL_FEATURE_D32_S8_SUPPORTED_BIT;
			}
		} else {
			switch (ePrecission) {
				case RE_DEPTH_PRECISSION_LOW:
					return DEPTH_STENCIL_FEATURE_D16_SUPPORTED_BIT;
				case RE_DEPTH_PRECISSION_MODERATE:
					return DEPTH_STENCIL_FEATURE_D24_SUPPORTED_BIT;
				case RE_DEPTH_PRECISSION_HIGH:
					return DEPTH_STENCIL_FEATURE_D32_SUPPORTED_BIT;
			}
		}
		RE_ABORT("Depth-stencil feature couldn't be determined: ", std::hex, ePrecission, ", ", bStencils);
	}

	static VkFormat get_depth_stencil_format(DepthPrecission ePrecission, bool bStencils) {
		if (bStencils) {
			switch (ePrecission) {
				case RE_DEPTH_PRECISSION_LOW:
					return VK_FORMAT_D16_UNORM_S8_UINT;
				case RE_DEPTH_PRECISSION_MODERATE:
					return VK_FORMAT_D24_UNORM_S8_UINT;
				case RE_DEPTH_PRECISSION_HIGH:
					return VK_FORMAT_D32_SFLOAT_S8_UINT;
			}
		} else {
			switch (ePrecission) {
				case RE_DEPTH_PRECISSION_LOW:
					return VK_FORMAT_D16_UNORM;
				case RE_DEPTH_PRECISSION_MODERATE:
					return VK_FORMAT_X8_D24_UNORM_PACK32;
				case RE_DEPTH_PRECISSION_HIGH:
					return VK_FORMAT_D32_SFLOAT;
			}
		}
		RE_ABORT("Depth-stencil feature couldn't be determined: ", std::hex, ePrecission, ", ", bStencils);
	}

	static void adjust_depth_stencil_mode(DepthPrecission &reDepthPrecission, bool &rbStencilsEnabled) {
		PRINT_DEBUG("Adjusting depth precission and enabled stencils to supported features and formats");
		const bool bStencilsRequired = rbStencilsEnabled;
		int iNextDepthPrecission;
		for (int iStencilIter = 0; iStencilIter < 2; iStencilIter++) {
			if (!bStencilsRequired || rbStencilsEnabled) {
				for (int iDepthIter = 0; iDepthIter < 3; iDepthIter++) {
					iNextDepthPrecission = static_cast<int>(reDepthPrecission) + iDepthIter;
					if (iNextDepthPrecission >= 3)
						iNextDepthPrecission = static_cast<int>(reDepthPrecission) - (iDepthIter - (2 - static_cast<int>(reDepthPrecission)));
					if (mDepthStencilFeatures & get_depth_stencil_feature(static_cast<DepthPrecission>(iNextDepthPrecission), rbStencilsEnabled))
						goto COMPATIBLE_DEPTH_STENCIL_MODE_FOUND;
				}
			}
			rbStencilsEnabled = !rbStencilsEnabled;
		}
		RE_ABORT("No suitable depth precission found. Depth precission was ", std::hex, reDepthPrecission, ", stencils enabled was ", std::dec, rbStencilsEnabled, " and feature flags were set to ", std::hex, mDepthStencilFeatures);

	COMPATIBLE_DEPTH_STENCIL_MODE_FOUND:
		PRINT_DEBUG("Writing adjusted depth precission");
		reDepthPrecission = static_cast<DepthPrecission>(iNextDepthPrecission);
		if (reDepthPrecission == RE_DEPTH_PRECISSION_MODERATE && !rbStencilsEnabled && (mDepthStencilFeatures & DEPTH_STENCIL_FEATURE_D32_SUPPORTED_BIT)) {
			PRINT_DEBUG("Depth precission will be upgraded from moderate to high quality without stencils (D24 -> D32)");
			reDepthPrecission = RE_DEPTH_PRECISSION_HIGH;
		} else if (reDepthPrecission == RE_DEPTH_PRECISSION_LOW && rbStencilsEnabled && (mDepthStencilFeatures & DEPTH_STENCIL_FEATURE_D24_S8_SUPPORTED_BIT)) {
			PRINT_DEBUG("Depth precission will be upgraded from low to moderate quality without stencils (D16_S8 -> D24_S8)");
			reDepthPrecission = RE_DEPTH_PRECISSION_MODERATE;
		}
	}

	void find_suitable_depth_stencil_formats() {
		PRINT_DEBUG("Setting data structures up for checking availability of Vulkan formats");
		VkPhysicalDeviceImageFormatInfo2 vk_imageFormatInfo = {
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2,
			.pNext = nullptr,
			.type = VK_IMAGE_TYPE_2D,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = vk_mDepthStencilImageUsages,
			.flags = 0
		};
		VkImageFormatProperties2 vk_imageFormatProperties2;
		vk_imageFormatProperties2.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2;
		vk_imageFormatProperties2.pNext = nullptr;
		const VkImageFormatProperties &vk_rImageFormatProperties = vk_imageFormatProperties2.imageFormatProperties;
		bool bDepthSupported,
			bStencilsSupported;
		constexpr VkFormat vk_aeDepthStencilFormats[] = {
			VK_FORMAT_D16_UNORM,
			VK_FORMAT_X8_D24_UNORM_PACK32,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D32_SFLOAT_S8_UINT
		};
		for (int iTransientAttachmentIter = 0; iTransientAttachmentIter < 2; iTransientAttachmentIter++) {
			switch (iTransientAttachmentIter) {
				case 0:
					vk_imageFormatInfo.usage |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
					break;
				case 1:
					vk_imageFormatInfo.usage &= ~VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
					break;
				[[unlikely]] default:
					RE_ABORT("The iteration index on image usage flags for depth and stencil images is unknown: ", std::hex, iTransientAttachmentIter);
			}
			mDepthStencilFeatures = 0;
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
							PRINT_DEBUG("Vulkan format ", std::hex, vk_eDepthStencilFormat, " is supported. Adding to flags of supported depth-stencil formats");
							switch (vk_eDepthStencilFormat) {
								case VK_FORMAT_D16_UNORM:
									mDepthStencilFeatures |= DEPTH_STENCIL_FEATURE_D16_SUPPORTED_BIT;
									bDepthSupported = true;
									break;
								case VK_FORMAT_X8_D24_UNORM_PACK32:
									mDepthStencilFeatures |= DEPTH_STENCIL_FEATURE_D24_SUPPORTED_BIT;
									bDepthSupported = true;
									break;
								case VK_FORMAT_D32_SFLOAT:
									mDepthStencilFeatures |= DEPTH_STENCIL_FEATURE_D32_SUPPORTED_BIT;
									bDepthSupported = true;
									break;
								case VK_FORMAT_D16_UNORM_S8_UINT:
									mDepthStencilFeatures |= DEPTH_STENCIL_FEATURE_D16_S8_SUPPORTED_BIT;
									bDepthSupported = true;
									bStencilsSupported = true;
									break;
								case VK_FORMAT_D24_UNORM_S8_UINT:
									mDepthStencilFeatures |= DEPTH_STENCIL_FEATURE_D24_S8_SUPPORTED_BIT;
									bDepthSupported = true;
									bStencilsSupported = true;
									break;
								case VK_FORMAT_D32_SFLOAT_S8_UINT:
									mDepthStencilFeatures |= DEPTH_STENCIL_FEATURE_D32_S8_SUPPORTED_BIT;
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
					mDepthStencilFeatures |= DEPTH_STENCIL_FEATURE_TRANSIENT_ATTACHMENT_BIT;
				}
				break;
			}
		}
		DepthPrecission eDepthPrecission = get_depth_precission();
		bool bStencilsEnabled = are_stencils_enabled();
		adjust_depth_stencil_mode(eDepthPrecission, bStencilsEnabled);
		set_depth_stencil_settings(eDepthPrecission, bStencilsEnabled);
	}

	bool create_depth_stencil_image() {
		const VkImageCreateInfo vk_depthStencilCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = vk_eSelectedDepthStencilFormat,
			.extent = {
				.width = renderImageSize[0],
				.height = renderImageSize[1],
				.depth = 1
			},
			.mipLevels = 1,
			.arrayLayers = RE_VK_FRAMES_IN_FLIGHT,
			.samples = vk_eMsaaMode,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = vk_mDepthStencilImageUsages | ((mDepthStencilFeatures & DEPTH_STENCIL_FEATURE_TRANSIENT_ATTACHMENT_BIT) ? VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT : 0),
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 1,
			.pQueueFamilyIndices = nullptr,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
		};
		PRINT_DEBUG("Creating Vulkan image for depth and stencils");
		if (vkCreateImage(vk_hDevice, &vk_depthStencilCreateInfo, nullptr, &vk_hDepthStencilImage) == VK_SUCCESS) {
			return true;
		} else
			RE_FATAL_ERROR("Failed to create Vulkan image with depth-stencil format ", std::hex, vk_depthStencilCreateInfo.format);
		vk_hDepthStencilImage = VK_NULL_HANDLE;
		return false;
	}

	bool create_depth_stencil_image_views() {
		PRINT_DEBUG("Setting data structures up for creating Vulkan image views referring depth-stencil data");
		VkImageViewCreateInfo vk_depthStencilViewCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.image = vk_hDepthStencilImage,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = vk_eSelectedDepthStencilFormat,
			.components = {
				.r = VK_COMPONENT_SWIZZLE_IDENTITY,
				.g = VK_COMPONENT_SWIZZLE_IDENTITY,
				.b = VK_COMPONENT_SWIZZLE_IDENTITY,
				.a = VK_COMPONENT_SWIZZLE_IDENTITY
			},
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | static_cast<VkImageAspectFlags>(are_stencils_enabled() ? VK_IMAGE_ASPECT_STENCIL_BIT : 0),
				.baseMipLevel = 0,
				.levelCount = 1,
				.layerCount = 1,
			}
		};
		unsigned uFrameInFlightCreateIndex;
		for (uFrameInFlightCreateIndex = 0; uFrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT; uFrameInFlightCreateIndex++) {
			PRINT_DEBUG("Creating Vulkan image view for depth-stencil at frame-in-flight index ", uFrameInFlightCreateIndex);
			vk_depthStencilViewCreateInfo.subresourceRange.baseArrayLayer = static_cast<uint32_t>(uFrameInFlightCreateIndex);
			if (vkCreateImageView(vk_hDevice, &vk_depthStencilViewCreateInfo, nullptr, &vk_ahDepthStencilImageViews[uFrameInFlightCreateIndex]) == VK_SUCCESS) {
				continue;
			} else
				RE_FATAL_ERROR("Failed creating Vulkan image view for depth-stencil at frame-in-flight index ", uFrameInFlightCreateIndex);
			vk_ahDepthStencilImageViews[uFrameInFlightCreateIndex] = VK_NULL_HANDLE;
			break;
		}
		if (uFrameInFlightCreateIndex == RE_VK_FRAMES_IN_FLIGHT) {
			return true;
		}
		for (unsigned uFrameInFlightDestroyIndex = 0; uFrameInFlightDestroyIndex < uFrameInFlightCreateIndex; uFrameInFlightDestroyIndex++) {
			PRINT_DEBUG("Destroying Vulkan image view at index ", uFrameInFlightDestroyIndex, " pointing at depth data due to failure creating all image views pointing at depth and stencil images");
			vkDestroyImageView(vk_hDevice, vk_ahDepthStencilImageViews[uFrameInFlightDestroyIndex], nullptr);
			vk_ahDepthStencilImageViews[uFrameInFlightDestroyIndex] = VK_NULL_HANDLE;
		}
		return false;
	}

	void destroy_depth_stencil_image() {
		PRINT_DEBUG("Destroying Vulkan depth-stencil image");
		vkDestroyImage(vk_hDevice, vk_hDepthStencilImage, nullptr);
		vk_hDepthStencilImage = VK_NULL_HANDLE;
	}

	void destroy_depth_stencil_image_views() {
		PRINT_DEBUG("Destroying Vulkan image views pointing to depth-stencil image");
		for (VkImageView &vk_rhDepthStencilImageView : vk_ahDepthStencilImageViews) {
			vkDestroyImageView(vk_hDevice, vk_rhDepthStencilImageView, nullptr);
			vk_rhDepthStencilImageView = VK_NULL_HANDLE;
		}
	}

	void set_depth_stencil_settings(DepthPrecission eNewDepthPrecission, bool bNewStencilsEnabled) {
		adjust_depth_stencil_mode(eNewDepthPrecission, bNewStencilsEnabled);
		const VkFormat vk_eNewDepthStencilFormat = get_depth_stencil_format(eNewDepthPrecission, bNewStencilsEnabled);
		if (vk_eNewDepthStencilFormat == vk_eSelectedDepthStencilFormat)
			return;
		if (bRunning) {
			PRINT_DEBUG("Destroying depth and stencil Vulkan images");
			wait_for_rendering_finished();
			destroy_renderer_images();
		}
		PRINT_DEBUG("Updating depth and stencil settings");
		vk_eSelectedDepthStencilFormat = vk_eNewDepthStencilFormat;
		if (bRunning) {
			PRINT_DEBUG("Recreating depth and stencil Vulkan images");
			create_renderer_images();
		}
	}

	void set_depth_precission(DepthPrecission eNewDepthPrecission) {
		set_depth_stencil_settings(eNewDepthPrecission, are_stencils_enabled());
	}

	void set_stencils_enabled(bool bNewStencilsEnabled) {
		set_depth_stencil_settings(get_depth_precission(), bNewStencilsEnabled);
	}

	DepthPrecission get_depth_precission() {
		switch (vk_eSelectedDepthStencilFormat) {
			case VK_FORMAT_D16_UNORM:
			case VK_FORMAT_D16_UNORM_S8_UINT:
				return RE_DEPTH_PRECISSION_LOW;
			case VK_FORMAT_X8_D24_UNORM_PACK32:
			case VK_FORMAT_D24_UNORM_S8_UINT:
				return RE_DEPTH_PRECISSION_MODERATE;
			case VK_FORMAT_D32_SFLOAT:
			case VK_FORMAT_D32_SFLOAT_S8_UINT:
				return RE_DEPTH_PRECISSION_HIGH;
			default:
				RE_ABORT("An invalid depth-stencil format is selected: ", std::hex, vk_eSelectedDepthStencilFormat);
		}
	}

	bool are_stencils_enabled() {
		switch (vk_eSelectedDepthStencilFormat) {
			case VK_FORMAT_D16_UNORM_S8_UINT:
			case VK_FORMAT_D24_UNORM_S8_UINT:
			case VK_FORMAT_D32_SFLOAT_S8_UINT:
				return true;
			default:
				return false;
		}
	}

}
