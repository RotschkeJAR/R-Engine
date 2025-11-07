#include "RE_SpriteLayout.hpp"
#include "RE_GPU.hpp"
#include "RE_Main.hpp"
#include "RE_Sprite.hpp"
#include "RE_Renderer.hpp"

namespace RE {

	[[nodiscard]]
	static VkFilter get_vulkan_filter(const TextureFilter eFilter) {
		switch (eFilter) {
			case RE_TEXTURE_FILTER_NEAREST:
				return VK_FILTER_NEAREST;
			case RE_TEXTURE_FILTER_LINEAR:
				return VK_FILTER_LINEAR;
			default:
				RE_FATAL_ERROR("Unknown texture filter ", std::hex, eFilter);
				std::abort();
		}
	}

	[[nodiscard]]
	static VkSamplerMipmapMode get_vulkan_mipmap_mode(const TextureFilter eFilter) {
		switch (eFilter) {
			case RE_TEXTURE_FILTER_NEAREST:
				return VK_SAMPLER_MIPMAP_MODE_NEAREST;
			case RE_TEXTURE_FILTER_LINEAR:
				return VK_SAMPLER_MIPMAP_MODE_LINEAR;
			default:
				RE_FATAL_ERROR("Unknown texture filter ", std::hex, eFilter, " for mipmaps");
				std::abort();
		}
	}

	[[nodiscard]]
	static VkSamplerAddressMode get_vulkan_sampler_address_mode(const TextureRepetition eRepitition) {
		switch (eRepitition) {
			case RE_TEXTURE_REPETITION_REPEAT:
				return VK_SAMPLER_ADDRESS_MODE_REPEAT;
			case RE_TEXTURE_REPETITION_MIRRORED_REPEAT:
				return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			case RE_TEXTURE_REPETITION_CLAMP_TO_EDGE:
				return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			case RE_TEXTURE_REPETITION_CLAMP_TO_BORDER:
				return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			default:
				RE_FATAL_ERROR("Unknown texture repetition ", std::hex, eRepitition);
				std::abort();
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
			default:
				RE_FATAL_ERROR("Unknown border color ", std::hex, eColor);
				std::abort();
		}
	}

	static bool create_vulkan_sampler_for_sprite_layout(const SpriteLayoutSettings &rSettings, VkSampler &vk_rhSamplerOut) {
		PRINT_DEBUG("Creating Vulkan sampler for sprite layout");
		VkSamplerCreateInfo vk_spriteLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.magFilter = get_vulkan_filter(rSettings.eMagFilter),
			.minFilter = get_vulkan_filter(rSettings.eMinFilter),
			.mipmapMode = get_vulkan_mipmap_mode(rSettings.eMipmapFilter),
			.addressModeU = get_vulkan_sampler_address_mode(rSettings.eTextureRepetitionU),
			.addressModeV = get_vulkan_sampler_address_mode(rSettings.eTextureRepetitionV),
			.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.mipLodBias = 0.0f,
			.compareEnable = VK_FALSE,
			.minLod = 0.0f,
			.maxLod = 0.0f,
			.borderColor = get_vulkan_border_color(rSettings.eBorderColor),
			.unnormalizedCoordinates = VK_FALSE
		};
		if (rSettings.fMaxAnisotropy >= 1.0f) {
			vk_spriteLayoutCreateInfo.anisotropyEnable = VK_TRUE;
			if (get_maximum_allowed_anisotropy() >= 1.0f && get_maximum_allowed_anisotropy() < rSettings.fMaxAnisotropy) {
				RE_NOTE("The GPU doesn't support any higher anisotropy than ", get_maximum_allowed_anisotropy(), ". Yours was set to ", rSettings.fMaxAnisotropy, " and has been clamped down to the limit");
				vk_spriteLayoutCreateInfo.maxAnisotropy = get_maximum_allowed_anisotropy();
			} else
				vk_spriteLayoutCreateInfo.maxAnisotropy = rSettings.fMaxAnisotropy;
		} else
			vk_spriteLayoutCreateInfo.anisotropyEnable = VK_FALSE;
		return vkCreateSampler(vk_hDevice, &vk_spriteLayoutCreateInfo, nullptr, &vk_rhSamplerOut) == VK_SUCCESS;
	}

	[[nodiscard]]
	SpriteLayout create_sprite_layout(const SpriteLayoutSettings &rSettings) {
		if (!vk_hDevice) {
			RE_ERROR("Sprite layouts cannot be created, when the engine is not running");
			return nullptr;
		}
		PRINT_DEBUG("Creating sprite layout");
		VulkanSpriteLayout *const pVulkanSpriteLayout = new VulkanSpriteLayout;
		if (!create_vulkan_sampler_for_sprite_layout(rSettings, pVulkanSpriteLayout->vk_hSampler)) {
			RE_FATAL_ERROR("Failed to create sprite layout");
			delete pVulkanSpriteLayout;
			return nullptr;
		}
		return reinterpret_cast<SpriteLayout>(pVulkanSpriteLayout);
	}

	bool change_sprite_layout_settings(const SpriteLayout hSpriteLayout, const SpriteLayoutSettings &rNewSettings) {
		if (!hSpriteLayout) {
			RE_NOTE("The settings of a null sprite layout cannot be changed. The engine will drop this request");
			return false;
		} else if (!vk_hDevice) {
			RE_ERROR("Sprite layouts aren't valid anymore, when the engine doesn't run, so their settings cannot be changed either");
			return false;
		}
		PRINT_DEBUG("Recreating Vulkan sampler in sprite layout ", hSpriteLayout);
		VulkanSpriteLayout *const pVulkanSpriteLayout = reinterpret_cast<VulkanSpriteLayout*>(hSpriteLayout);
		const VkSampler vk_hOldSampler = pVulkanSpriteLayout->vk_hSampler;
		if (create_vulkan_sampler_for_sprite_layout(rNewSettings, pVulkanSpriteLayout->vk_hSampler)) {
			vkDestroySampler(vk_hDevice, vk_hOldSampler, nullptr);
			std::vector<VkDescriptorImageInfo> textureInfos;
			std::vector<VkWriteDescriptorSet> updateInfos;
			textureInfos.reserve(std::clamp<uint16_t>(vulkanSprites.size() / 3, 1, std::numeric_limits<uint16_t>::max()));
			updateInfos.reserve(textureInfos.capacity());
			for (uint16_t u16UniformIndex = 0; u16UniformIndex < vulkanSprites.size(); u16UniformIndex++) {
				if (vulkanSprites[u16UniformIndex]->pLayout != pVulkanSpriteLayout)
					continue;
				textureInfos.emplace_back();
				textureInfos.back().sampler = pVulkanSpriteLayout->vk_hSampler;
				textureInfos.back().imageView = vulkanSprites[u16UniformIndex]->pTexture->vk_hImageView;
				textureInfos.back().imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				updateInfos.emplace_back();
				updateInfos.back().sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				updateInfos.back().pNext = nullptr;
				updateInfos.back().dstSet = vk_hTextureDescSet;
				updateInfos.back().dstBinding = RE_VK_UNIFORM_BINDING_TEXTURE;
				updateInfos.back().dstArrayElement = u16UniformIndex;
				updateInfos.back().descriptorCount = 1;
				updateInfos.back().descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				updateInfos.back().pImageInfo = const_cast<const VkDescriptorImageInfo*>(&textureInfos.back());
				updateInfos.back().pBufferInfo = nullptr;
				updateInfos.back().pTexelBufferView = nullptr;
			}
			wait_for_rendering_finished();
			vkUpdateDescriptorSets(vk_hDevice, updateInfos.size(), updateInfos.data(), 0, nullptr);
			return true;
		} else {
			RE_ERROR("Failed recreating Vulkan sampler in sprite layout ", hSpriteLayout, ". The old sampler object will be kept");
			pVulkanSpriteLayout->vk_hSampler = vk_hOldSampler;
			return false;
		}
	}

	void destroy_sprite_layout(const SpriteLayout hSpriteLayout) {
		if (!hSpriteLayout) {
			RE_NOTE("A null sprite layout had to be destroyed. The engine ignores that request");
			return;
		} else if (!vk_hDevice) {
			RE_ERROR("Sprite layouts aren't valid anymore, when the engine doesn't run, so they cannot be destroyed either");
			RE_NOTE("The engine will attempt to free some memory used for sprite layout ", hSpriteLayout);
			delete reinterpret_cast<const VulkanSpriteLayout*>(hSpriteLayout);
			return;
		}
		PRINT_DEBUG("Destroying sprite layout ", hSpriteLayout);
		const VulkanSpriteLayout *const pVulkanSpriteLayout = reinterpret_cast<VulkanSpriteLayout*>(hSpriteLayout);
		vkDestroySampler(vk_hDevice, pVulkanSpriteLayout->vk_hSampler, nullptr);
		delete pVulkanSpriteLayout;
#ifndef RE_DISABLE_PRINT_DEBUGS
		for (VulkanSprite *const pSprite : vulkanSprites)
			if (pSprite->pLayout == pVulkanSpriteLayout)
				RE_WARNING("There might be a dangling pointer in sprite ", reinterpret_cast<Sprite>(pSprite), " after sprite layout ", hSpriteLayout, " has been destroyed! Destroy the sprite or assign it a valid sprite layout to avoid undefined behaviour or severe crashes");
#endif
	}

	float get_maximum_allowed_anisotropy() {
		return vk_physicalDeviceProperties.limits.maxSamplerAnisotropy;
	}

}
