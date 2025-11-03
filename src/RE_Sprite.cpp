#include "RE_Sprite.hpp"
#include "RE_Renderer.hpp"

namespace RE {

	std::deque<VulkanSprite*> vulkanSprites;

	[[nodiscard]]
	Sprite create_sprite(const Texture hTexture, const SpriteLayout hSpriteLayout) {
		if (!hTexture || !hSpriteLayout) {
			RE_ERROR("Texture and sprite layout cannot be null to create a sprite");
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
		const VkDescriptorImageInfo vk_spriteInfo = {
			.sampler = pSprite->pLayout->vk_hSampler,
			.imageView = pSprite->pTexture->vk_hImageView,
			.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};
		const VkWriteDescriptorSet vk_writeSpriteInfo = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = vk_ahPermanentDescSets[0],
			.dstBinding = RE_VK_UNIFORM_BINDING_TEXTURE,
			.dstArrayElement = pSprite->u16UniformIndex,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo = &vk_spriteInfo
		};
		constexpr uint8_t u8CopyAmount = RE_VK_FRAMES_IN_FLIGHT - 1;
		std::array<VkCopyDescriptorSet, u8CopyAmount> copySpriteInfos;
		for (uint8_t u8CopyInfoIndex = 0; u8CopyInfoIndex < u8CopyAmount; u8CopyInfoIndex++) {
			copySpriteInfos[u8CopyInfoIndex].sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
			copySpriteInfos[u8CopyInfoIndex].pNext = nullptr;
			copySpriteInfos[u8CopyInfoIndex].srcSet = vk_ahPermanentDescSets[0];
			copySpriteInfos[u8CopyInfoIndex].srcBinding = RE_VK_UNIFORM_BINDING_TEXTURE;
			copySpriteInfos[u8CopyInfoIndex].srcArrayElement = pSprite->u16UniformIndex;
			copySpriteInfos[u8CopyInfoIndex].dstSet = vk_ahPermanentDescSets[u8CopyInfoIndex + 1];
			copySpriteInfos[u8CopyInfoIndex].dstBinding = RE_VK_UNIFORM_BINDING_TEXTURE;
			copySpriteInfos[u8CopyInfoIndex].dstArrayElement = pSprite->u16UniformIndex;
			copySpriteInfos[u8CopyInfoIndex].descriptorCount = 1;
		}
		wait_for_rendering_finished();
		vkUpdateDescriptorSets(vk_hDevice, 1, &vk_writeSpriteInfo, u8CopyAmount, copySpriteInfos.data());
		return reinterpret_cast<Sprite>(pSprite);
	}

	void change_texture_in_sprite(const Sprite hSprite, const Texture hTexture) {
		VulkanSprite *const pSprite = reinterpret_cast<VulkanSprite*>(hSprite);
		pSprite->pTexture = reinterpret_cast<VulkanTexture*>(hTexture);
		const VkDescriptorImageInfo vk_spriteInfo = {
			.sampler = pSprite->pLayout->vk_hSampler,
			.imageView = pSprite->pTexture->vk_hImageView,
			.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};
		const VkWriteDescriptorSet vk_writeSpriteInfo = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = vk_ahPermanentDescSets[0],
			.dstBinding = RE_VK_UNIFORM_BINDING_TEXTURE,
			.dstArrayElement = pSprite->u16UniformIndex,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo = &vk_spriteInfo
		};
		constexpr uint8_t u8CopyAmount = RE_VK_FRAMES_IN_FLIGHT - 1;
		std::array<VkCopyDescriptorSet, u8CopyAmount> copySpriteInfos;
		for (uint8_t u8CopyInfoIndex = 0; u8CopyInfoIndex < u8CopyAmount; u8CopyInfoIndex++) {
			copySpriteInfos[u8CopyInfoIndex].sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
			copySpriteInfos[u8CopyInfoIndex].pNext = nullptr;
			copySpriteInfos[u8CopyInfoIndex].srcSet = vk_ahPermanentDescSets[0];
			copySpriteInfos[u8CopyInfoIndex].srcBinding = RE_VK_UNIFORM_BINDING_TEXTURE;
			copySpriteInfos[u8CopyInfoIndex].srcArrayElement = pSprite->u16UniformIndex;
			copySpriteInfos[u8CopyInfoIndex].dstSet = vk_ahPermanentDescSets[u8CopyInfoIndex + 1];
			copySpriteInfos[u8CopyInfoIndex].dstBinding = RE_VK_UNIFORM_BINDING_TEXTURE;
			copySpriteInfos[u8CopyInfoIndex].dstArrayElement = pSprite->u16UniformIndex;
			copySpriteInfos[u8CopyInfoIndex].descriptorCount = 1;
		}
		wait_for_rendering_finished();
		vkUpdateDescriptorSets(vk_hDevice, 1, &vk_writeSpriteInfo, u8CopyAmount, copySpriteInfos.data());
	}

	void change_layout_in_sprite(const Sprite hSprite, const SpriteLayout hSpriteLayout) {
		VulkanSprite *const pSprite = reinterpret_cast<VulkanSprite*>(hSprite);
		pSprite->pLayout = reinterpret_cast<VulkanSpriteLayout*>(hSpriteLayout);
		const VkDescriptorImageInfo vk_spriteInfo = {
			.sampler = pSprite->pLayout->vk_hSampler,
			.imageView = pSprite->pTexture->vk_hImageView,
			.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		};
		const VkWriteDescriptorSet vk_writeSpriteInfo = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = vk_ahPermanentDescSets[0],
			.dstBinding = RE_VK_UNIFORM_BINDING_TEXTURE,
			.dstArrayElement = pSprite->u16UniformIndex,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo = &vk_spriteInfo
		};
		constexpr uint8_t u8CopyAmount = RE_VK_FRAMES_IN_FLIGHT - 1;
		std::array<VkCopyDescriptorSet, u8CopyAmount> copySpriteInfos;
		for (uint8_t u8CopyInfoIndex = 0; u8CopyInfoIndex < u8CopyAmount; u8CopyInfoIndex++) {
			copySpriteInfos[u8CopyInfoIndex].sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
			copySpriteInfos[u8CopyInfoIndex].pNext = nullptr;
			copySpriteInfos[u8CopyInfoIndex].srcSet = vk_writeSpriteInfo.dstSet;
			copySpriteInfos[u8CopyInfoIndex].srcBinding = RE_VK_UNIFORM_BINDING_TEXTURE;
			copySpriteInfos[u8CopyInfoIndex].srcArrayElement = vk_writeSpriteInfo.dstArrayElement;
			copySpriteInfos[u8CopyInfoIndex].dstSet = vk_ahPermanentDescSets[u8CopyInfoIndex + 1];
			copySpriteInfos[u8CopyInfoIndex].dstBinding = RE_VK_UNIFORM_BINDING_TEXTURE;
			copySpriteInfos[u8CopyInfoIndex].dstArrayElement = pSprite->u16UniformIndex;
			copySpriteInfos[u8CopyInfoIndex].descriptorCount = 1;
		}
		wait_for_rendering_finished();
		vkUpdateDescriptorSets(vk_hDevice, 1, &vk_writeSpriteInfo, u8CopyAmount, copySpriteInfos.data());
	}

	void destroy_sprite(const Sprite hSprite) {
		const VulkanSprite *const pSprite = reinterpret_cast<const VulkanSprite*>(hSprite);
		vulkanSprites.back()->u16UniformIndex = pSprite->u16UniformIndex;
		vulkanSprites[pSprite->u16UniformIndex] = vulkanSprites.back();
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
