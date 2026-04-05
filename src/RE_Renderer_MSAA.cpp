#include "RE_Renderer_Internal.hpp"
#include "RE_Main.hpp"

namespace RE {
	
	VkSampleCountFlagBits vk_eMsaaCount = VK_SAMPLE_COUNT_1_BIT;

	void set_msaa_mode(const MsaaMode eNewMsaaMode) {
		MsaaMode eNextMsaaMode = eNewMsaaMode;
		while (!is_msaa_mode_supported(eNextMsaaMode) && eNextMsaaMode != RE_MSAA_MODE_1) {
			PRINT_DEBUG("MSAA mode ", eNextMsaaMode, " not supported. Dropping it down by one");
			eNextMsaaMode = static_cast<MsaaMode>(static_cast<uint8_t>(eNextMsaaMode) - 1);
		}
		const VkSampleCountFlagBits vk_eNewSampleCount = static_cast<VkSampleCountFlagBits>(VK_SAMPLE_COUNT_1_BIT << eNextMsaaMode);
		if (vk_eNewSampleCount == vk_eMsaaCount) {
			if (eNewMsaaMode != eNextMsaaMode)
				RE_WARNING("MSAA mode ", eNewMsaaMode, " is not supported on this GPU and has been dropped down to ", eNextMsaaMode, ", but is already set");
			return;
		} else if (eNewMsaaMode != eNextMsaaMode)
			RE_WARNING("MSAA mode ", eNewMsaaMode, " is not supported on this GPU and has been dropped down to ", eNextMsaaMode);
		PRINT_DEBUG("New MSAA mode ", eNextMsaaMode, " has been saved as ", std::hex, vk_eNewSampleCount);
		vk_eMsaaCount = vk_eNewSampleCount;
		if (bRunning) {
			PRINT_DEBUG("Recreating render images and render pipeline to adjust to new MSAA mode");
			vkDeviceWaitIdle(vk_hDevice);
			recreate_graphics_pipelines();
			destroy_images_renderer();
			create_images_renderer();
		}
	}

	[[nodiscard]]
	MsaaMode get_msaa_mode() {
		switch (vk_eMsaaCount) {
			case VK_SAMPLE_COUNT_1_BIT:
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
			[[unlikely]] default:
				RE_ABORT("Unknown MSAA mode is selected in the settings: ", std::hex, vk_eMsaaCount);
		}
	}

	[[nodiscard]]
	bool is_msaa_mode_supported(const MsaaMode eMsaaMode) {
		return ((VK_SAMPLE_COUNT_1_BIT << eMsaaMode) & vk_mSupportedMsaaSamples) != 0;
	}

	void get_supported_msaa_modes(const uint8_t u8ListLength, MsaaMode *const paeSupportedMsaaModes, uint8_t *const pu8SupportedMsaaModeCount) {
		if ((!u8ListLength || !paeSupportedMsaaModes) && !pu8SupportedMsaaModeCount)
			return;
		uint8_t u8Index = 0;
		for (uint8_t u8MsaaModeIndex = 0; u8MsaaModeIndex <= static_cast<uint8_t>(RE_MSAA_MODE_64); u8MsaaModeIndex++) {
			PRINT_DEBUG("Checking availability of MSAA mode ", u8MsaaModeIndex);
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
			PRINT_DEBUG("Writing count of available MSAA modes (", u8Index, ") to ", pu8SupportedMsaaModeCount);
			*pu8SupportedMsaaModeCount = u8Index;
		}
	}

	[[nodiscard]]
	MsaaMode get_highest_supported_msaa_mode() {
		for (int8_t i8MsaaModeIndex = static_cast<int8_t>(RE_MSAA_MODE_64); i8MsaaModeIndex > 0; i8MsaaModeIndex--) {
			PRINT_DEBUG("Checking availability of MSAA mode ", i8MsaaModeIndex);
			const MsaaMode eMsaaMode = static_cast<MsaaMode>(i8MsaaModeIndex);
			if (is_msaa_mode_supported(eMsaaMode)) {
				PRINT_DEBUG("MSAA mode ", i8MsaaModeIndex, " is the highest supported mode by the GPU");
				return eMsaaMode;
			}
		}
		return RE_MSAA_MODE_1;
	}

}
