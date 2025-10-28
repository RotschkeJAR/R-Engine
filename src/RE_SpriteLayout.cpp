#include "RE_Internal Header.hpp"
#include "RE_Vulkan_Device.hpp"
#include "RE_GPU.hpp"
#include "RE_Main.hpp"

namespace RE {

	[[nodiscard]]
	static VkFilter get_vulkan_filter(const TextureFilter eFilter) {
		switch (eFilter) {
			case RE_TEXTURE_FILTER_NEAREST:
				return VK_FILTER_NEAREST;
			case RE_TEXTURE_FILTER_LINEAR:
				return VK_FILTER_LINEAR;
		}
	}

	[[nodiscard]]
	static VkSamplerMipmapMode get_vulkan_mipmap_mode(const TextureFilter eFilter) {
		switch (eFilter) {
			case RE_TEXTURE_FILTER_NEAREST:
				return VK_SAMPLER_MIPMAP_MODE_NEAREST;
			case RE_TEXTURE_FILTER_LINEAR:
				return VK_SAMPLER_MIPMAP_MODE_LINEAR;
		}
	}

	[[nodiscard]]
	static VkSamplerAddressMode get_vulkan_sampler_address_mode(const TextureRepitition eRepitition) {
		switch (eRepitition) {
			case RE_TEXTURE_REPETITION_REPEAT:
				return VK_SAMPLER_ADDRESS_MODE_REPEAT;
			case RE_TEXTURE_REPETITION_MIRRORED_REPEAT:
				return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			case RE_TEXTURE_REPETITION_CLAMP_TO_EDGE:
				return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			case RE_TEXTURE_REPETITION_CLAMP_TO_BORDER:
				return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		}
	}

	[[nodiscard]]
	static VkBorderColor get_vulkan_border_color(const BorderColor eColor) {
		switch (eColor) {
			case RE_BORDER_COLOR_TRANSPARENT:
				return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
			case RE_BORDER_COLOR_BLACK:
				return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
			case RE_BORDER_COLOR_WHITE:
				return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		}
	}
	
	[[nodiscard]]
	SpriteLayout create_sprite_layout() {
		PRINT_DEBUG("Creating sprite layout with default settings");
		const SpriteLayoutSettings initialSpriteLayoutSettings;
		return create_sprite_layout(initialSpriteLayoutSettings);
	}

	[[nodiscard]]
	SpriteLayout create_sprite_layout(const SpriteLayoutSettings &rSettings) {
		if (!bRunning) {
			RE_ERROR("Sprite layouts cannot be created, when the engine is not running");
			return nullptr;
		}
		PRINT_DEBUG("Creating sprite layout");
		VkSamplerCreateInfo vk_spriteLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.flags = 0,
			.magFilter = get_vulkan_filter(rSettings.eMagFilter),
			.minFilter = get_vulkan_filter(rSettings.eMinFilter),
			.mipmapMode = get_vulkan_mipmap_mode(rSettings.eMipmapFilter),
			.addressModeU = get_vulkan_sampler_address_mode(rSettings.eTextureRepetitionU),
			.addressModeV = get_vulkan_sampler_address_mode(rSettings.eTextureRepetitionV),
			.mipLodBias = 0.0f,
			.compareEnable = VK_FALSE,
			.minLod = 0.0f,
			.maxLod = 0.0f,
			.borderColor = get_vulkan_border_color(rSettings.eBorderColor)
		};
		if (rSettings.fMaxAnisotropy >= 1.0f) {
			vk_spriteLayoutCreateInfo.anisotropyEnable = VK_TRUE;
			vk_spriteLayoutCreateInfo.maxAnisotropy = rSettings.fMaxAnisotropy;
		} else
			vk_spriteLayoutCreateInfo.anisotropyEnable = VK_FALSE;
		VkSampler vk_hSpriteLayout = VK_NULL_HANDLE;
		if (vkCreateSampler(vk_hDevice, &vk_spriteLayoutCreateInfo, nullptr, &vk_hSpriteLayout) != VK_SUCCESS)
			RE_FATAL_ERROR("Failed to create sprite layout");
		return reinterpret_cast<SpriteLayout>(vk_hSpriteLayout);
	}

	void change_sprite_layout_settings(SpriteLayout &rSpriteLayout, const SpriteLayoutSettings &rNewSettings) {
		if (!bRunning) {
			RE_ERROR("Sprite layouts aren't valid anymore, when the engine doesn't run, so their settings cannot be changed either");
			return;
		}
		PRINT_DEBUG("Recreating sprite layout at ", &rSpriteLayout, " to apply new settings");
		destroy_sprite_layout(rSpriteLayout);
		rSpriteLayout = create_sprite_layout(rNewSettings);
	}

	void destroy_sprite_layout(const SpriteLayout &rSpriteLayout) {
		if (!spriteLayout) {
			RE_NOTE("A null sprite layout had to be destroyed. The engine ignores that request");
			return;
		} else if (!bRunning) {
			RE_ERROR("Sprite layouts aren't valid anymore, when the engine doesn't run, so they cannot be destroyed either");
			return;
		}
		PRINT_DEBUG("Destroying sprite layout ", rSpriteLayout);
		vkDestroySampler(vk_hDevice, reinterpret_cast<VkSampler>(rSpriteLayout), nullptr);
		rSpriteLayout = nullptr;
	}

}
