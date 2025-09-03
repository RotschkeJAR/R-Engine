#include "RE_Renderer_Internal.hpp"

namespace RE {
	
	VkSampleCountFlagBits vk_eMsaaCount = VK_SAMPLE_COUNT_1_BIT;
	std::array<VkImage, RE_VK_FRAMES_IN_FLIGHT> vk_ahSingleSampledWorldRenderImages = {};
	std::array<VkDeviceMemory, RE_VK_FRAMES_IN_FLIGHT> vk_ahSingleSampledWorldRenderImageMemories = {};

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
		if (vk_hDevice == VK_NULL_HANDLE) {
			vk_eMsaaCount = vk_eNewSampleCount;
			return;
		}
		WAIT_FOR_IDLE_VULKAN_DEVICE();
		/*PUSH_TO_CALLSTACKTRACE(destroy_world_render_images());
		vkDestroyRenderPass(vk_hDevice, vk_hWorldRenderPass, nullptr);
		vk_eMsaaCount = vk_eNewSampleCount;
		if (create_render_pass()) {
			if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_world_render_images(), bool)) {
				if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(recreate_game_object_render_pipelines(), bool))
					return;
				PUSH_TO_CALLSTACKTRACE(destroy_world_render_images());
			}
			vkDestroyRenderPass(vk_hDevice, vk_hWorldRenderPass, nullptr);
		}
		vk_hWorldRenderPass = VK_NULL_HANDLE;*/
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
		const VkSampleCountFlags vk_eAllowedSamples = vk_physicalDeviceProperties.limits.framebufferColorSampleCounts & vk_physicalDeviceProperties.limits.framebufferDepthSampleCounts & vk_physicalDeviceProperties.limits.framebufferStencilSampleCounts;
		return ((VK_SAMPLE_COUNT_1_BIT << eMsaaMode) & vk_eAllowedSamples) != 0;
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
