#include "RE_Renderer_SpriteLayout.hpp"
#include "RE_Main.hpp"

namespace RE {

	static std::jthread nextSpriteLayoutIndexSearchThread;
	static std::unique_ptr<VulkanSpriteLayout[]> vulkanSpriteLayouts;
	VkSampler vk_hDefaultSampler;
	static uint16_t u16MaxSpriteLayoutCount = 255,
		u16CurrentSpriteLayoutCount = 0,
		u16NextSpriteLayoutIndex = 0;

	bool init_renderer_sprite_layouts() {
		const VkSamplerCreateInfo vk_defaultSamplerCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.magFilter = VK_FILTER_LINEAR,
			.minFilter = VK_FILTER_LINEAR,
			.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
			.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
			.mipLodBias = 0.0f,
			.anisotropyEnable = VK_FALSE,
			.maxAnisotropy = 0.0f,
			.compareEnable = VK_FALSE,
			.compareOp = VK_COMPARE_OP_NEVER,
			.minLod = 0.0f,
			.maxLod = 0.0f,
			.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
			.unnormalizedCoordinates = VK_FALSE
		};
		if (vkCreateSampler(vk_hDevice, &vk_defaultSamplerCreateInfo, nullptr, &vk_hDefaultSampler) == VK_SUCCESS) {
			if (u16MaxSpriteLayoutCount) {
				vulkanSpriteLayouts = std::make_unique<VulkanSpriteLayout[]>(u16MaxSpriteLayoutCount);
				std::for_each(vulkanSpriteLayouts.get(), vulkanSpriteLayouts.get() + u16MaxSpriteLayoutCount, [](VulkanSpriteLayout &rSpriteLayout) {
					rSpriteLayout.vk_hSampler = VK_NULL_HANDLE;
				});
			}
			return true;
		}
		return false;
	}

	void destroy_renderer_sprite_layout() {
		vulkanSpriteLayouts.reset();
		u16CurrentSpriteLayoutCount = 0;
		if (nextSpriteLayoutIndexSearchThread.joinable())
			nextSpriteLayoutIndexSearchThread.join();
		u16NextSpriteLayoutIndex = 0;
		vkDestroySampler(vk_hDevice, vk_hDefaultSampler, nullptr);
	}

	uint16_t get_index_of_sprite_layout(const VulkanSpriteLayout *const pSpriteLayout) {
		return static_cast<uint16_t>(pSpriteLayout - vulkanSpriteLayouts.get());
	}

	static void search_next_sprite_layout_index(uint16_t u16Expiry) {
		while (true) {
			u16NextSpriteLayoutIndex = (u16NextSpriteLayoutIndex + 1) % u16MaxSpriteLayoutCount;
			if (!vulkanSpriteLayouts[u16NextSpriteLayoutIndex].vk_hSampler)
				break;
			else if (!u16Expiry)
				RE_ABORT("Search for the next free slot for sprite layout allocation expired after ", u16MaxSpriteLayoutCount, " attempts, when ", u16CurrentSpriteLayoutCount, " sprite layouts were allocated");
			u16Expiry--;
		}
	}

	static VkFilter get_vulkan_filter(const TextureFilter eFilter) {
		switch (eFilter) {
			case RE_TEXTURE_FILTER_NEAREST:
				return VK_FILTER_NEAREST;
			case RE_TEXTURE_FILTER_LINEAR:
				return VK_FILTER_LINEAR;
			default:
				RE_ABORT("Unknown texture filter ", std::hex, eFilter);
		}
	}

	static VkSamplerMipmapMode get_vulkan_mipmap_mode(const TextureFilter eFilter) {
		switch (eFilter) {
			case RE_TEXTURE_FILTER_NEAREST:
				return VK_SAMPLER_MIPMAP_MODE_NEAREST;
			case RE_TEXTURE_FILTER_LINEAR:
				return VK_SAMPLER_MIPMAP_MODE_LINEAR;
			default:
				RE_ABORT("Unknown texture filter ", std::hex, eFilter, " for mipmaps");
		}
	}

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
				RE_ABORT("Unknown texture repetition ", std::hex, eRepitition);
		}
	}

	static VkBorderColor get_vulkan_border_color(const BorderColor eColor) {
		switch (eColor) {
			case RE_BORDER_COLOR_TRANSPARENT:
				return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
			case RE_BORDER_COLOR_BLACK:
				return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
			case RE_BORDER_COLOR_WHITE:
				return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			default:
				RE_ABORT("Unknown border color ", std::hex, eColor);
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
		if (rSettings.f32MaxAnisotropy >= 1.0f && are_vulkan_features_enabled<ENABLED_FEATURE_ANISOTROPIC_FILTERING_BIT>()) {
			vk_spriteLayoutCreateInfo.anisotropyEnable = VK_TRUE;
			if (get_maximum_allowed_anisotropy() < rSettings.f32MaxAnisotropy) {
				RE_NOTE("The GPU doesn't support any higher anisotropy than ", get_maximum_allowed_anisotropy(), ". It was set to ", rSettings.f32MaxAnisotropy, " and has been clamped down to the limit");
				vk_spriteLayoutCreateInfo.maxAnisotropy = get_maximum_allowed_anisotropy();
			} else
				vk_spriteLayoutCreateInfo.maxAnisotropy = rSettings.f32MaxAnisotropy;
		} else
			vk_spriteLayoutCreateInfo.anisotropyEnable = VK_FALSE;
		return vkCreateSampler(vk_hDevice, &vk_spriteLayoutCreateInfo, nullptr, &vk_rhSamplerOut) == VK_SUCCESS;
	}

	static void update_texture_descriptor_set_for_sprite_layout(const VulkanSpriteLayout *const pVulkanSpriteLayout, const bool bWaitUnused, const bool bWaitUpdateAfterBind) {
		VkDescriptorImageInfo vk_samplerInfo;
		vk_samplerInfo.sampler = pVulkanSpriteLayout->vk_hSampler;
		const VkWriteDescriptorSet vk_updateInfo = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = nullptr,
			.dstSet = vk_hSpriteDescSet,
			.dstBinding = RE_VK_SPRITE_DESC_SET_SPRITE_LAYOUT_BINDING_INDEX,
			.dstArrayElement = get_index_of_sprite_layout(pVulkanSpriteLayout),
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
			.pImageInfo = &vk_samplerInfo,
			.pBufferInfo = nullptr,
			.pTexelBufferView = nullptr
		};
		if ((bWaitUnused && !are_vulkan_features_enabled<ENABLED_FEATURE_UPDATE_UNUSED_DESCRIPTORS_WHILE_PENDING_BIT>())
				|| (bWaitUpdateAfterBind && !are_vulkan_features_enabled<ENABLED_FEATURE_UPDATE_DESCRIPTOR_SAMPLED_IMAGE_AFTER_BIND_BIT>()))
			wait_for_rendering_finished();
		vkUpdateDescriptorSets(vk_hDevice, 1, &vk_updateInfo, 0, nullptr);
	}

	[[nodiscard]]
	SpriteLayout create_sprite_layout(const SpriteLayoutSettings &rSettings) {
		if (!bRunning) {
			RE_ERROR("Sprite layouts cannot be created, when the engine is not running");
			return nullptr;
		}
		if (nextSpriteLayoutIndexSearchThread.joinable()) {
			PRINT_DEBUG("Waiting for search for the next free sprite layout slot to finish");
			nextSpriteLayoutIndexSearchThread.join();
		}
		PRINT_DEBUG("Creating sprite layout");
		VulkanSpriteLayout *const pVulkanSpriteLayout = std::addressof(vulkanSpriteLayouts[u16NextSpriteLayoutIndex]);
		if (create_vulkan_sampler_for_sprite_layout(rSettings, pVulkanSpriteLayout->vk_hSampler)) {
			u16CurrentSpriteLayoutCount++;
			if (u16CurrentSpriteLayoutCount < u16MaxSpriteLayoutCount)
				nextSpriteLayoutIndexSearchThread = std::jthread(search_next_sprite_layout_index, u16MaxSpriteLayoutCount);
			update_texture_descriptor_set_for_sprite_layout(pVulkanSpriteLayout, true, false);
			return reinterpret_cast<SpriteLayout>(pVulkanSpriteLayout);
		} else
			RE_FATAL_ERROR("Failed to create sprite layout");
		return nullptr;
	}

	bool change_sprite_layout_settings(const SpriteLayout hSpriteLayout, const SpriteLayoutSettings &rNewSettings) {
		if (!hSpriteLayout) {
			RE_NOTE("The settings of a null sprite layout cannot be changed. The engine will drop this request");
			return false;
		} else if (!bRunning) {
			RE_ERROR("Sprite layouts aren't valid anymore, when the engine doesn't run, so their settings cannot be changed either");
			return false;
		}
		PRINT_DEBUG("Recreating Vulkan sampler in sprite layout ", hSpriteLayout);
		VulkanSpriteLayout *const pVulkanSpriteLayout = reinterpret_cast<VulkanSpriteLayout*>(hSpriteLayout);
		const VkSampler vk_hOldSampler = pVulkanSpriteLayout->vk_hSampler;
		if (create_vulkan_sampler_for_sprite_layout(rNewSettings, pVulkanSpriteLayout->vk_hSampler)) {
			update_texture_descriptor_set_for_sprite_layout(pVulkanSpriteLayout, false, true);
			return true;
		} else {
			RE_ERROR("Failed recreating Vulkan sampler in sprite layout ", hSpriteLayout, ". The old sampler object will be kept");
			pVulkanSpriteLayout->vk_hSampler = vk_hOldSampler;
			return false;
		}
	}

	void destroy_sprite_layout(const SpriteLayout hSpriteLayout) {
		if (!hSpriteLayout)
			RE_NOTE("A null sprite layout had to be destroyed. The engine ignores that request");
		else if (bRunning) {
			PRINT_DEBUG("Destroying sprite layout ", hSpriteLayout);
			VulkanSpriteLayout *const pVulkanSpriteLayout = reinterpret_cast<VulkanSpriteLayout*>(hSpriteLayout);
			vkDestroySampler(vk_hDevice, pVulkanSpriteLayout->vk_hSampler, nullptr);
			pVulkanSpriteLayout->vk_hSampler = VK_NULL_HANDLE;
			u16CurrentSpriteLayoutCount--;
		} else
			RE_ERROR("Sprite layouts aren't valid anymore, when the engine doesn't run, so they cannot be destroyed either");
	}

	[[nodiscard]]
	float get_maximum_allowed_anisotropy() {
		return f32MaxSamplerAnisotropy;
	}

	[[nodiscard]]
	uint16_t get_max_sprite_layout_count() {
		return u16MaxSpriteLayoutCount;
	}

	void set_max_sprite_layout_count(const uint16_t u16NewMaxSpriteLayoutCount) {
		if (!bRunning)
			RE_ERROR("The maximum count of sprite layouts cannot be changed while the engine is running");
		else
			u16MaxSpriteLayoutCount = u16NewMaxSpriteLayoutCount;
	}

	[[nodiscard]]
	uint16_t get_remaining_sprite_layout_allocs() {
		return u16MaxSpriteLayoutCount - u16CurrentSpriteLayoutCount;
	}

}
