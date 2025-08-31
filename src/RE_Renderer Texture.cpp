#include "RE_Renderer Texture.hpp"
#include "RE_Render System.hpp"
#include "RE_Renderer.hpp"
#include "RE_Internal Header.hpp"

namespace RE {

	uint32_t u16MaximumTextures;
	int16_t i16TextureInUseCounter = 0;
	std::array<VkDescriptorImageInfo, RE_VK_COUNT_OF_TEXTURE_DESCRIPTOR> vk_aTextureSamplersToDescriptorSet = {};
	
	void fetch_minimum_allowed_texture_count() {
		u16MaximumTextures = static_cast<uint16_t>(std::min<uint32_t>(std::min<uint32_t>(vk_physicalDeviceProperties.limits.maxPerStageDescriptorSamplers, vk_physicalDeviceProperties.limits.maxPerStageDescriptorSampledImages), RE_VK_MAX_SAMPLED_IMAGES));
	}

	void reset_texture_counter() {
		i16TextureInUseCounter = 0;
	}

	void submit_sprite(const Sprite *const pSprite, int32_t &ri32TextureIndex) {
		if (static_cast<uint16_t>(i16TextureInUseCounter) >= u16MaximumTextures) {
			ri32TextureIndex = -1;
			return;
		}
		PUSH_TO_CALLSTACKTRACE(vk_aTextureSamplersToDescriptorSet[i16TextureInUseCounter].sampler = reinterpret_cast<VkSampler>(pSprite->hSpriteLayout));
		PUSH_TO_CALLSTACKTRACE(vk_aTextureSamplersToDescriptorSet[i16TextureInUseCounter].imageView = reinterpret_cast<TextureContainer*>(pSprite->hTexture)->vk_hImgView);
		vk_aTextureSamplersToDescriptorSet[i16TextureInUseCounter].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		ri32TextureIndex = i16TextureInUseCounter;
		i16TextureInUseCounter++;
	}

	void submit_sprites_to_descriptor_set(const uint8_t u8FrameInFlightIndex) {
		/*const VkWriteDescriptorSet vk_writeToTextureDescriptorSet = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = vk_ahDescriptorSets[u8FrameInFlightIndex],
			.dstBinding = 1,
			.dstArrayElement = 0,
			.descriptorCount = static_cast<uint32_t>(i16TextureInUseCounter),
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.pImageInfo = vk_aTextureSamplersToDescriptorSet.data()
		};
		vkUpdateDescriptorSets(vk_hDevice, 1, &vk_writeToTextureDescriptorSet, 0, nullptr);*/
	}

}
