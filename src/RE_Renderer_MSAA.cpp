#include "RE_Renderer_Internal.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_GPU.hpp"

namespace RE {
	
	VkSampleCountFlags vk_eAllowedMsaaSamples;
	VkSampleCountFlagBits vk_eMsaaCount = VK_SAMPLE_COUNT_1_BIT;
	std::array<VkImage, RE_VK_FRAMES_IN_FLIGHT> vk_ahSingleSampledWorldRenderImages;
	std::array<VkDeviceMemory, RE_VK_FRAMES_IN_FLIGHT> vk_ahSingleSampledWorldRenderImageMemories;
	std::array<VkImageView, RE_VK_FRAMES_IN_FLIGHT> vk_ahSingleSampledWorldRenderImageViews;

	bool create_singlesampled_images(const VkExtent3D &vk_rSingleSampledImageExtent3D) {
		if (vk_eMsaaCount == VK_SAMPLE_COUNT_1_BIT)
			return true;
		size_t singleSampledImageCreateIndex = 0;
		while (singleSampledImageCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
			if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_image(0, VK_IMAGE_TYPE_2D, vk_eSwapchainImageFormat, vk_rSingleSampledImageExtent3D, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, 1, nullptr, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_ahSingleSampledWorldRenderImages[singleSampledImageCreateIndex], &vk_ahSingleSampledWorldRenderImageMemories[singleSampledImageCreateIndex]), bool)) {
				if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_image_view(vk_ahSingleSampledWorldRenderImages[singleSampledImageCreateIndex], VK_IMAGE_VIEW_TYPE_2D, vk_eSwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1, &vk_ahSingleSampledWorldRenderImageViews[singleSampledImageCreateIndex]), bool)) {
					singleSampledImageCreateIndex++;
					continue;
				}
				vkFreeMemory(vk_hDevice, vk_ahSingleSampledWorldRenderImageMemories[singleSampledImageCreateIndex], nullptr);
				vkDestroyImage(vk_hDevice, vk_ahSingleSampledWorldRenderImages[singleSampledImageCreateIndex], nullptr);
			}
			break;
		}
		if (singleSampledImageCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
			return true;
		for (size_t singleSampledImageDestroyIndex = 0; singleSampledImageDestroyIndex < singleSampledImageCreateIndex; singleSampledImageDestroyIndex++) {
			vkDestroyImageView(vk_hDevice, vk_ahSingleSampledWorldRenderImageViews[singleSampledImageDestroyIndex], nullptr);
			vkFreeMemory(vk_hDevice, vk_ahSingleSampledWorldRenderImageMemories[singleSampledImageDestroyIndex], nullptr);
			vkDestroyImage(vk_hDevice, vk_ahSingleSampledWorldRenderImages[singleSampledImageDestroyIndex], nullptr);
		}
		return false;
	}

	void destroy_singlesampled_images() {
		if (vk_eMsaaCount == VK_SAMPLE_COUNT_1_BIT)
			return;
		for (size_t singleSampledImageDestroyIndex = 0; singleSampledImageDestroyIndex < RE_VK_FRAMES_IN_FLIGHT; singleSampledImageDestroyIndex++) {
			vkDestroyImageView(vk_hDevice, vk_ahSingleSampledWorldRenderImageViews[singleSampledImageDestroyIndex], nullptr);
			vkFreeMemory(vk_hDevice, vk_ahSingleSampledWorldRenderImageMemories[singleSampledImageDestroyIndex], nullptr);
			vkDestroyImage(vk_hDevice, vk_ahSingleSampledWorldRenderImages[singleSampledImageDestroyIndex], nullptr);
		}
	}

	void set_msaa_mode(const MsaaMode eNewMsaaMode) {
		MsaaMode eNextMsaaMode = eNewMsaaMode;
		while (!is_msaa_mode_supported(eNextMsaaMode) && eNextMsaaMode != RE_MSAA_MODE_1)
			eNextMsaaMode = static_cast<MsaaMode>(static_cast<uint8_t>(eNextMsaaMode) - 1);
		const VkSampleCountFlagBits vk_eNewSampleCount = static_cast<VkSampleCountFlagBits>(VK_SAMPLE_COUNT_1_BIT << eNextMsaaMode);
		if (vk_eNewSampleCount == vk_eMsaaCount) {
			if (eNewMsaaMode != eNextMsaaMode)
				RE_WARNING("MSAA mode ", std::pow(2, static_cast<int32_t>(eNewMsaaMode)), " is not supported on this GPU and has been dropped down to ", std::pow(2, static_cast<int32_t>(eNextMsaaMode)), ", but is already set");
			return;
		} else if (eNewMsaaMode != eNextMsaaMode)
			RE_WARNING("MSAA mode ", std::pow(2, static_cast<int32_t>(eNewMsaaMode)), " is not supported on this GPU and has been dropped down to ", std::pow(2, static_cast<int32_t>(eNextMsaaMode)));
		vk_eMsaaCount = vk_eNewSampleCount;
		if (vk_hDevice == VK_NULL_HANDLE)
			return;
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
		if (!paeSupportedMsaaModes || !u8ListLength)
			return;
		uint8_t u8Index = 0;
		for (uint8_t u8MsaaModeIndex = 0; u8MsaaModeIndex < static_cast<uint8_t>(RE_MSAA_MODE_64) && u8Index < u8ListLength; u8MsaaModeIndex++) {
			const MsaaMode eMsaaMode = static_cast<MsaaMode>(u8MsaaModeIndex);
			if (is_msaa_mode_supported(eMsaaMode)) {
				if (paeSupportedMsaaModes)
					paeSupportedMsaaModes[u8Index] = eMsaaMode;
				u8Index++;
			}
		}
		if (pu8SupportedMsaaModeCount)
			*pu8SupportedMsaaModeCount = u8Index;
	}

	[[nodiscard]]
	MsaaMode get_highest_supported_msaa_mode() {
		for (uint8_t u8MsaaModeIndex = static_cast<uint8_t>(RE_MSAA_MODE_64); u8MsaaModeIndex > 0; u8MsaaModeIndex--) {
			const MsaaMode eMsaaMode = static_cast<MsaaMode>(u8MsaaModeIndex);
			if (is_msaa_mode_supported(eMsaaMode))
				return eMsaaMode;
		}
		return RE_MSAA_MODE_1;
	}

}
