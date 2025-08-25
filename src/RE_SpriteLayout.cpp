#include "RE_Internal Header.hpp"
#include "RE_Vulkan_Device.hpp"
#include "RE_Render System.hpp"

namespace RE {

	static VkFilter to_vk_filter(const TextureFilter eTexFilter, bool &rbFailure, const char *pacFilterName) {
		switch (eTexFilter) {
			case RE_TEXTURE_FILTER_NEAREST:
				return VK_FILTER_NEAREST;
			case RE_TEXTURE_FILTER_LINEAR:
				return VK_FILTER_LINEAR;
			default:
				RE_ERROR("Unknown ", pacFilterName, "-texture filter: ", hexadecimal_to_string(static_cast<size_t>(eTexFilter)));
				rbFailure = true;
				return VK_FILTER_LINEAR;
		}
	}

	static VkSamplerMipmapMode to_vk_sampler_mipmap_mode(const TextureFilter eTexFilter, bool &rbFailure) {
		switch (eTexFilter) {
			case RE_TEXTURE_FILTER_NEAREST:
				return VK_SAMPLER_MIPMAP_MODE_NEAREST;
			case RE_TEXTURE_FILTER_LINEAR:
				return VK_SAMPLER_MIPMAP_MODE_LINEAR;
			default:
				RE_ERROR("Unknown mipmap-texture filter: ", hexadecimal_to_string(static_cast<size_t>(eTexFilter)));
				rbFailure = true;
				return VK_SAMPLER_MIPMAP_MODE_LINEAR;
		}
	}

	static VkSamplerAddressMode to_vk_sampler_address_mode(const TextureRepetition eTexRepeat, bool &rbFailure, const char cAxis) {
		switch (eTexRepeat) {
			case RE_TEXTURE_REPETITION_REPEAT:
				return VK_SAMPLER_ADDRESS_MODE_REPEAT;
			case RE_TEXTURE_REPETITION_MIRRORED_REPEAT:
				return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			case RE_TEXTURE_REPETITION_CLAMP_TO_EDGE:
				return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			case RE_TEXTURE_REPETITION_CLAMP_TO_BORDER:
				return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			default:
				RE_ERROR("Unknown texture repetition mode along the ", cAxis, "-axis: ", hexadecimal_to_string(static_cast<size_t>(eTexRepeat)));
				rbFailure = true;
				return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		}
	}

	static VkBorderColor to_vk_border_color(const BorderColor eBorderColor, bool &rbFailure) {
		switch (eBorderColor) {
			case RE_BORDER_COLOR_TRANSPARENT:
				return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
			case RE_BORDER_COLOR_BLACK:
				return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
			case RE_BORDER_COLOR_WHITE:
				return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			default:
				RE_ERROR("Unknown border color selected: ", hexadecimal_to_string(static_cast<size_t>(eBorderColor)));
				rbFailure = true;
				return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		}
	}
	
	SpriteLayout create_sprite_layout() {
		const SpriteLayoutSettings initialSpriteLayoutSettings;
		return PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_sprite_layout(initialSpriteLayoutSettings), SpriteLayout);
	}

	SpriteLayout create_sprite_layout(const SpriteLayoutSettings &rSettings) {
		if (vk_hDevice == VK_NULL_HANDLE) {
			RE_ERROR("Sprite layouts cannot be created, when the engine doesn't run");
			return nullptr;
		}
		bool bFailure = false;
		VkSamplerCreateInfo vk_samplerCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.magFilter = to_vk_filter(rSettings.eMagFilter, bFailure, "magnification"),
			.minFilter = to_vk_filter(rSettings.eMinFilter, bFailure, "miniaturization"),
			.mipmapMode = to_vk_sampler_mipmap_mode(rSettings.eMipmapFilter, bFailure),
			.addressModeU = to_vk_sampler_address_mode(rSettings.eTextureRepetitionU, bFailure, 'U'),
			.addressModeV = to_vk_sampler_address_mode(rSettings.eTextureRepetitionV, bFailure, 'V'),
			.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.mipLodBias = 0.0f,
			.compareEnable = VK_FALSE,
			.compareOp = VK_COMPARE_OP_ALWAYS,
			.minLod = 0.0f,
			.maxLod = 0.0f,
			.borderColor = to_vk_border_color(rSettings.eBorderColor, bFailure),
			.unnormalizedCoordinates = VK_FALSE
		};
		if (bFailure)
			return nullptr;
		if (rSettings.fMaxAnisotropy >= 1.0f) {
			if (vk_physicalDeviceFeatures.samplerAnisotropy == VK_TRUE) {
				vk_samplerCreateInfo.anisotropyEnable = VK_TRUE;
				vk_samplerCreateInfo.maxAnisotropy = rSettings.fMaxAnisotropy;
			} else {
				RE_WARNING("The GPU doesn't support anisotropic filtering, therefore the maximum anisotropy of the sprite layout will be disabled");
				vk_samplerCreateInfo.anisotropyEnable = VK_FALSE;
			}
		} else
			vk_samplerCreateInfo.anisotropyEnable = VK_FALSE;
		VkSampler vk_hSampler;
		return vkCreateSampler(vk_hDevice, &vk_samplerCreateInfo, nullptr, &vk_hSampler) == VK_SUCCESS ? reinterpret_cast<SpriteLayout>(vk_hSampler) : nullptr;
	}

	void change_sprite_layout_settings(SpriteLayout &rSpriteLayout, const SpriteLayoutSettings &rNewSettings) {
		if (vk_hDevice == VK_NULL_HANDLE) {
			RE_ERROR("Settings of a sprite layout cannot be changed, when the engine doesn't run");
			return;
		}
		PUSH_TO_CALLSTACKTRACE(destroy_sprite_layout(rSpriteLayout));
		rSpriteLayout = PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_sprite_layout(rNewSettings), SpriteLayout);
	}

	void destroy_sprite_layout(const SpriteLayout spriteLayout) {
		if (!spriteLayout)
			return;
		else if (vk_hDevice == VK_NULL_HANDLE) {
			RE_ERROR("Sprite layouts cannot be destroyed anymore, when the engine doesn't run");
			return;
		}
		WAIT_FOR_IDLE_VULKAN_DEVICE();
		vkDestroySampler(vk_hDevice, reinterpret_cast<VkSampler>(spriteLayout), nullptr);
	}

}
