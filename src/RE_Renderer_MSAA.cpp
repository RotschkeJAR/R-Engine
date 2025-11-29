#include "RE_Renderer_Internal.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_GPU.hpp"
#include "RE_Main.hpp"

namespace RE {
	
	VkSampleCountFlags vk_eAllowedMsaaSamples;
	VkSampleCountFlagBits vk_eMsaaCount = VK_SAMPLE_COUNT_1_BIT;
	VkImage vk_hSingleSampledWorldRenderImages;
	static VkDeviceMemory vk_hSingleSampledWorldRenderImageMemories;
	VkImageView vk_ahSingleSampledWorldRenderImageViews[RE_VK_FRAMES_IN_FLIGHT];

	bool create_singlesampled_images(const std::vector<uint32_t> &rRenderQueuesFamilyIndices, const bool bResolvingRequired, const bool bBlittingRequired) {
		if (!bResolvingRequired || !bBlittingRequired)
			return true;
		PRINT_DEBUG("Creating Vulkan image for rendering multisampled");
		if (create_vulkan_image(
				0,
				VK_IMAGE_TYPE_2D,
				vk_eSwapchainImageFormat,
				VkExtent3D{vk_renderImageSize.width, vk_renderImageSize.height, 1},
				1,
				RE_VK_FRAMES_IN_FLIGHT,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
				rRenderQueuesFamilyIndices.size(),
				rRenderQueuesFamilyIndices.data(),
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&vk_hSingleSampledWorldRenderImages,
				&vk_hSingleSampledWorldRenderImageMemories)) {
			uint8_t u8FrameInFlightIndex = 0;
			while (u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT) {
				if (create_vulkan_image_view(
						vk_hSingleSampledWorldRenderImages,
						VK_IMAGE_VIEW_TYPE_2D,
						vk_eSwapchainImageFormat,
						VK_IMAGE_ASPECT_COLOR_BIT,
						0,
						1,
						u8FrameInFlightIndex,
						1,
						&vk_ahSingleSampledWorldRenderImageViews[u8FrameInFlightIndex])) {
					u8FrameInFlightIndex++;
					continue;
				} else
					RE_FATAL_ERROR("Failed creating Vulkan image view at index ", u8FrameInFlightIndex, " pointing at singlesampled image");
				break;
			}
			if (u8FrameInFlightIndex == RE_VK_FRAMES_IN_FLIGHT)
				return true;
			else
				for (uint8_t u8SingleSampledImageViewDestroyIndex = 0; u8SingleSampledImageViewDestroyIndex < u8FrameInFlightIndex; u8SingleSampledImageViewDestroyIndex++)
					vkDestroyImageView(vk_hDevice, vk_ahSingleSampledWorldRenderImageViews[u8SingleSampledImageViewDestroyIndex], nullptr);
		} else
			RE_FATAL_ERROR("Failed to create singlesampled Vulkan image");
		return false;
	}

	void destroy_singlesampled_images() {
		PRINT_DEBUG("Destroying singlesampled Vulkan image ", vk_hSingleSampledWorldRenderImages, " and its memory ", vk_hSingleSampledWorldRenderImageMemories);
		vkFreeMemory(vk_hDevice, vk_hSingleSampledWorldRenderImageMemories, nullptr);
		vkDestroyImage(vk_hDevice, vk_hSingleSampledWorldRenderImages, nullptr);
	}

	void set_msaa_mode(const MsaaMode eNewMsaaMode) {
		MsaaMode eNextMsaaMode = eNewMsaaMode;
		while (!is_msaa_mode_supported(eNextMsaaMode) && eNextMsaaMode != RE_MSAA_MODE_1) {
			PRINT_DEBUG("MSAA-mode ", eNextMsaaMode, " not supported. Dropping it down by one");
			eNextMsaaMode = static_cast<MsaaMode>(static_cast<uint8_t>(eNextMsaaMode) - 1);
		}
		const VkSampleCountFlagBits vk_eNewSampleCount = static_cast<VkSampleCountFlagBits>(VK_SAMPLE_COUNT_1_BIT << eNextMsaaMode);
		if (vk_eNewSampleCount == vk_eMsaaCount) {
			if (eNewMsaaMode != eNextMsaaMode)
				RE_WARNING("MSAA mode ", std::pow(2, static_cast<int32_t>(eNewMsaaMode)), " is not supported on this GPU and has been dropped down to ", std::pow(2, static_cast<int32_t>(eNextMsaaMode)), ", but is already set");
			return;
		} else if (eNewMsaaMode != eNextMsaaMode)
			RE_WARNING("MSAA mode ", std::pow(2, static_cast<int32_t>(eNewMsaaMode)), " is not supported on this GPU and has been dropped down to ", std::pow(2, static_cast<int32_t>(eNextMsaaMode)));
		PRINT_DEBUG("New MSAA-mode ", eNextMsaaMode, " has been saved as ", std::hex, vk_eNewSampleCount);
		vk_eMsaaCount = vk_eNewSampleCount;
		bRenderPipelinesDirty = bRunning;
	}

	[[nodiscard]]
	MsaaMode get_msaa_mode() {
		switch (vk_eMsaaCount) {
			case VK_SAMPLE_COUNT_1_BIT:
			default:
				return RE_MSAA_MODE_1;
			case VK_SAMPLE_COUNT_2_BIT:
				return RE_MSAA_MODE_2;
			case VK_SAMPLE_COUNT_4_BIT:
				return RE_MSAA_MODE_4;
			case VK_SAMPLE_COUNT_8_BIT:
				return RE_MSAA_MODE_8;
			case VK_SAMPLE_COUNT_16_BIT:
				return RE_MSAA_MODE_16;
			case VK_SAMPLE_COUNT_32_BIT:
				return RE_MSAA_MODE_32;
			case VK_SAMPLE_COUNT_64_BIT:
				return RE_MSAA_MODE_64;
		}
	}

	[[nodiscard]]
	bool is_msaa_mode_supported(const MsaaMode eMsaaMode) {
		return ((VK_SAMPLE_COUNT_1_BIT << eMsaaMode) & vk_eAllowedMsaaSamples) != 0;
	}

	void get_supported_msaa_modes(const uint8_t u8ListLength, MsaaMode *const paeSupportedMsaaModes, uint8_t *const pu8SupportedMsaaModeCount) {
		if ((!u8ListLength || !paeSupportedMsaaModes) && !pu8SupportedMsaaModeCount)
			return;
		uint8_t u8Index = 0;
		for (uint8_t u8MsaaModeIndex = 0; u8MsaaModeIndex <= static_cast<uint8_t>(RE_MSAA_MODE_64); u8MsaaModeIndex++) {
			PRINT_DEBUG("Checking availability of MSAA-mode ", u8MsaaModeIndex);
			const MsaaMode eMsaaMode = static_cast<MsaaMode>(u8MsaaModeIndex);
			if (is_msaa_mode_supported(eMsaaMode)) {
				if (paeSupportedMsaaModes && u8Index < u8ListLength) {
					PRINT_DEBUG("Writing available MSAA-mode ", eMsaaMode, " to array ", paeSupportedMsaaModes, " at index ", u8Index);
					paeSupportedMsaaModes[u8Index] = eMsaaMode;
				}
				u8Index++;
			}
		}
		if (pu8SupportedMsaaModeCount) {
			PRINT_DEBUG("Writing count of available MSAA-modes (", u8Index, ") to ", pu8SupportedMsaaModeCount);
			*pu8SupportedMsaaModeCount = u8Index;
		}
	}

	[[nodiscard]]
	MsaaMode get_highest_supported_msaa_mode() {
		for (uint8_t u8MsaaModeIndex = static_cast<uint8_t>(RE_MSAA_MODE_64); u8MsaaModeIndex > 0; u8MsaaModeIndex--) {
			PRINT_DEBUG("Checking availability of MSAA-mode ", u8MsaaModeIndex);
			const MsaaMode eMsaaMode = static_cast<MsaaMode>(u8MsaaModeIndex);
			if (is_msaa_mode_supported(eMsaaMode)) {
				PRINT_DEBUG("MSAA-mode ", u8MsaaModeIndex, " is the highest supported mode by the GPU");
				return eMsaaMode;
			}
		}
		return RE_MSAA_MODE_1;
	}

}
