#include "RE_Sprite.hpp"
#include "RE_Renderer.hpp"

namespace RE {

	std::deque<VulkanSprite*> vulkanSprites;

	static void write_to_texture_descriptor_set(const VulkanSprite &rSprite) {
		const VkDescriptorImageInfo vk_spriteInfo = {
			.sampler = rSprite.pLayout->vk_hSampler,
			.imageView = rSprite.pTexture->vk_hImageView,
			.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};
		const VkWriteDescriptorSet vk_writeSpriteInfo = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = vk_hTextureDescSet,
			.dstBinding = RE_VK_UNIFORM_BINDING_TEXTURE,
			.dstArrayElement = rSprite.u16UniformIndex,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo = &vk_spriteInfo
		};
		vkUpdateDescriptorSets(vk_hDevice, 1, &vk_writeSpriteInfo, 0, nullptr);
	}

	[[nodiscard]]
	Sprite create_sprite(const Texture hTexture, const SpriteLayout hSpriteLayout) {
		if (!hTexture || !hSpriteLayout) {
			RE_ERROR("Texture and sprite layout cannot be null to create a sprite");
			return nullptr;
		} else if (!vk_hDevice) {
			RE_ERROR("Sprites cannot be created, when the engine is not running");
			return nullptr;
		} else if (vulkanSprites.size() >= RE_VK_MAX_SAMPLED_IMAGES) {
			RE_FATAL_ERROR("The limit of ", std::hex, RE_VK_MAX_SAMPLED_IMAGES, " simultaneous existing sprites has been surpassed");
			return nullptr;
		}
		VulkanSprite *const pSprite = new VulkanSprite;
		pSprite->pTexture = reinterpret_cast<VulkanTexture*>(hTexture);
		pSprite->pLayout = reinterpret_cast<VulkanSpriteLayout*>(hSpriteLayout);
		pSprite->u16UniformIndex = static_cast<uint16_t>(vulkanSprites.size());
		vulkanSprites.push_back(pSprite);
		write_to_texture_descriptor_set(*pSprite);
		return reinterpret_cast<Sprite>(pSprite);
	}

	void change_texture_in_sprite(const Sprite hSprite, const Texture hTexture) {
		VulkanSprite *const pSprite = reinterpret_cast<VulkanSprite*>(hSprite);
		pSprite->pTexture = reinterpret_cast<VulkanTexture*>(hTexture);
		wait_for_rendering_finished();
		write_to_texture_descriptor_set(*pSprite);
	}

	void change_layout_in_sprite(const Sprite hSprite, const SpriteLayout hSpriteLayout) {
		VulkanSprite *const pSprite = reinterpret_cast<VulkanSprite*>(hSprite);
		pSprite->pLayout = reinterpret_cast<VulkanSpriteLayout*>(hSpriteLayout);
		wait_for_rendering_finished();
		write_to_texture_descriptor_set(*pSprite);
	}

	void destroy_sprite(const Sprite hSprite) {
		if (!hSprite) {
			RE_NOTE("A null sprite had to be destroyed. The engine ignores that request");
			return;
		} else if (!vk_hDevice) {
			RE_ERROR("Sprites aren't valid anymore, when the engine doesn't run, so they cannot be destroyed either");
			RE_NOTE("The engine will attempt to free some memory used for sprite ", hSprite);
			delete reinterpret_cast<const VulkanSprite*>(hSprite);
			return;
		}
		const VulkanSprite *const pSprite = reinterpret_cast<const VulkanSprite*>(hSprite);
		if (vulkanSprites.back() != pSprite) {
			const VkCopyDescriptorSet vk_copyBackSprite = {
				.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET,
				.srcSet = vk_hTextureDescSet,
				.srcBinding = RE_VK_UNIFORM_BINDING_TEXTURE,
				.srcArrayElement = vulkanSprites.back()->u16UniformIndex,
				.dstSet = vk_hTextureDescSet,
				.dstBinding = RE_VK_UNIFORM_BINDING_TEXTURE,
				.dstArrayElement = pSprite->u16UniformIndex,
				.descriptorCount = 1
			};
			vulkanSprites.back()->u16UniformIndex = pSprite->u16UniformIndex;
			vulkanSprites[pSprite->u16UniformIndex] = vulkanSprites.back();
			vulkanSprites.pop_back();
			wait_for_rendering_finished();
			vkUpdateDescriptorSets(vk_hDevice, 0, nullptr, 1, &vk_copyBackSprite);
		} else
			vulkanSprites.pop_back();
		delete pSprite;
	}

	[[nodiscard]]
	Texture get_texture_from_sprite(const Sprite hSprite) {
		return reinterpret_cast<Texture>(reinterpret_cast<VulkanSprite*>(hSprite)->pTexture);
	}

	[[nodiscard]]
	SpriteLayout get_sprite_layout_from_sprite(const Sprite hSprite) {
		return reinterpret_cast<SpriteLayout>(reinterpret_cast<VulkanSprite*>(hSprite)->pLayout);
	}
	
}
