#include "RE_Texture.hpp"
#include "RE_Vulkan_Device.hpp"
#include "RE_GPU.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_List_GameObject.hpp"
#include "RE_Renderer.hpp"
#include "RE_Sprite.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#define STBI_ASSERT(x)
#include "stb_image.h"

namespace RE {

	[[nodiscard]]
	Texture alloc_texture_from_binary_data(const uint8_t *const pau8TextureBinaries, const uint32_t u32Width, const uint32_t u32Height, const uint32_t u32Channels) {
		if (!pau8TextureBinaries || !u32Width || !u32Height || !u32Channels) {
			RE_ERROR("Textures cannot be allocated, when its binaries are null or width, height or number of channels are zero");
			return nullptr;
		} else if (!vk_hDevice) {
			RE_ERROR("Textures cannot be created, when the engine is not running");
			return nullptr;
		}
		PRINT_DEBUG("Allocating new texture");
		VulkanTexture *const pVulkanTexture = new VulkanTexture;
		switch (u32Channels) {
			case 1:
				pVulkanTexture->vk_eFormat = VK_FORMAT_R8_SRGB;
				break;
			case 2:
				pVulkanTexture->vk_eFormat = VK_FORMAT_R8G8_SRGB;
				break;
			case 3:
				pVulkanTexture->vk_eFormat = VK_FORMAT_R8G8B8_SRGB;
				break;
			case 4:
				pVulkanTexture->vk_eFormat = VK_FORMAT_R8G8B8A8_SRGB;
				break;
			[[unlikely]] default:
				RE_ERROR("The texture contains supposedly ", u32Channels, " channels, which is unknown to the engine. Therefore the texture won't be allocated");
				delete pVulkanTexture;
				return nullptr;
		}
		PRINT_DEBUG("Creating temporary staging Vulkan buffer for transferring texture binaries");
		const VkDeviceSize vk_imageBufferSize = sizeof(uint8_t) * u32Width * u32Height * u32Channels;
		Vulkan_Buffer stagingImageBuffer(vk_imageBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 1, nullptr, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		if (stagingImageBuffer.is_valid()) {
			uint8_t *pau8StagingBufferContent;
			if (stagingImageBuffer.map_memory(0, vk_imageBufferSize, reinterpret_cast<void**>(&pau8StagingBufferContent))) {
				std::jthread textureCopyThread([&]() {
					std::memcpy(pau8StagingBufferContent, pau8TextureBinaries, vk_imageBufferSize);
				});
				PRINT_DEBUG("Creating Vulkan image for storing texture on GPU");
				if (create_vulkan_image(0, VK_IMAGE_TYPE_2D, pVulkanTexture->vk_eFormat, VkExtent3D{u32Width, u32Height, 1}, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 1, nullptr, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &pVulkanTexture->vk_hImage, &pVulkanTexture->vk_hImageMemory)) {
					PRINT_DEBUG("Assigning commands and recording Vulkan task for transferring texture to GPU");
					const auto transferFunc = [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
						const VkImageMemoryBarrier2 vk_imageLayoutBarrier = {
							.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
							.srcStageMask = VK_PIPELINE_STAGE_2_NONE,
							.srcAccessMask = VK_ACCESS_2_NONE,
							.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
							.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
							.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
							.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
							.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
							.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
							.image = pVulkanTexture->vk_hImage,
							.subresourceRange = {
								.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
								.baseMipLevel = 0,
								.levelCount = 1,
								.baseArrayLayer = 0,
								.layerCount = 1
							}
						};
						const VkDependencyInfo vk_transitImageLayoutInfo = {
							.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
							.imageMemoryBarrierCount = 1,
							.pImageMemoryBarriers = &vk_imageLayoutBarrier
						};
						vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_transitImageLayoutInfo);
						const VkBufferImageCopy2 vk_copyRegion = {
							.sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2,
							.bufferOffset = 0,
							.bufferRowLength = 0,
							.bufferImageHeight = 0,
							.imageSubresource = {
								.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
								.mipLevel = 0,
								.baseArrayLayer = 0,
								.layerCount = 1
							},
							.imageExtent = {
								.width = u32Width,
								.height = u32Height,
								.depth = 1
							}
						};
						const VkCopyBufferToImageInfo2 vk_copyInfo = {
							.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2,
							.srcBuffer = stagingImageBuffer.get_buffer(),
							.dstImage = pVulkanTexture->vk_hImage,
							.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
							.regionCount = 1,
							.pRegions = &vk_copyRegion,
						};
						vkCmdCopyBufferToImage2(vk_hCommandBuffer, &vk_copyInfo);
						VkImageMemoryBarrier2 vk_imageLayoutFinalBarrier;
						vk_imageLayoutFinalBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
						vk_imageLayoutFinalBarrier.pNext = nullptr;
						vk_imageLayoutFinalBarrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
						vk_imageLayoutFinalBarrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
						vk_imageLayoutFinalBarrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
						vk_imageLayoutFinalBarrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
						vk_imageLayoutFinalBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
						vk_imageLayoutFinalBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						vk_imageLayoutFinalBarrier.image = pVulkanTexture->vk_hImage;
						vk_imageLayoutFinalBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
						vk_imageLayoutFinalBarrier.subresourceRange.baseMipLevel = 0;
						vk_imageLayoutFinalBarrier.subresourceRange.levelCount = 1;
						vk_imageLayoutFinalBarrier.subresourceRange.baseArrayLayer = 0;
						vk_imageLayoutFinalBarrier.subresourceRange.layerCount = 1;
						if (u8CurrentLogicalQueue == u8NextLogicalQueue) {
							vk_imageLayoutFinalBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
							vk_imageLayoutFinalBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
						} else {
							vk_imageLayoutFinalBarrier.srcQueueFamilyIndex = queueFamilyIndices[u8CurrentLogicalQueue];
							vk_imageLayoutFinalBarrier.dstQueueFamilyIndex = queueFamilyIndices[u8NextLogicalQueue];
						}
						const VkDependencyInfo vk_transitImageLayoutFinalInfo = {
							.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
							.imageMemoryBarrierCount = 1,
							.pImageMemoryBarriers = &vk_imageLayoutFinalBarrier
						};
						vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_transitImageLayoutFinalInfo);
					};
					const auto ownershipAcquireFunc = [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
						if (u8PreviousLogicalQueue == u8CurrentLogicalQueue)
							return;
						const VkImageMemoryBarrier2 vk_inheritImageBarrier = {
							.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
							.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
							.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
							.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
							.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT,
							.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
							.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
							.srcQueueFamilyIndex = queueFamilyIndices[u8PreviousLogicalQueue],
							.dstQueueFamilyIndex = queueFamilyIndices[u8CurrentLogicalQueue],
							.image = pVulkanTexture->vk_hImage,
							.subresourceRange = {
								.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
								.baseMipLevel = 0,
								.levelCount = 1,
								.baseArrayLayer = 0,
								.layerCount = 1
							}
						};
						const VkDependencyInfo vk_inheritImageInfo = {
							.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
							.imageMemoryBarrierCount = 1,
							.pImageMemoryBarriers = &vk_inheritImageBarrier
						};
						vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_inheritImageInfo);
					};
					PRINT_DEBUG("Submitting Vulkan task for transferring texture to GPU");
					textureCopyThread.join();
					submit_transfer_task(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, transferFunc, ownershipAcquireFunc, 0, nullptr, 0, nullptr, VK_NULL_HANDLE);
					if (create_vulkan_image_view(pVulkanTexture->vk_hImage, VK_IMAGE_VIEW_TYPE_2D, pVulkanTexture->vk_eFormat, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1, &pVulkanTexture->vk_hImageView)) {
						pVulkanTexture->a2u32Size[0] = u32Width;
						pVulkanTexture->a2u32Size[1] = u32Height;
						wait_for_transfer(std::numeric_limits<uint64_t>::max());
						return reinterpret_cast<Texture>(pVulkanTexture);
					} else
						RE_FATAL_ERROR("Failed to create Vulkan image view for image ", pVulkanTexture->vk_hImage);
					PRINT_DEBUG("Destroying Vulkan image ", pVulkanTexture->vk_hImage, " and freeing its memory ", pVulkanTexture->vk_hImageMemory, " on GPU due to failure allocating essential resources for the texture");
					vkDestroyImage(vk_hDevice, pVulkanTexture->vk_hImage, nullptr);
					vkFreeMemory(vk_hDevice, pVulkanTexture->vk_hImageMemory, nullptr);
				} else
					RE_FATAL_ERROR("Failed to create Vulkan image on GPU to permanently store the texture");
			} else
				RE_FATAL_ERROR("Failed to map temporary Vulkan buffer ", stagingImageBuffer.get_buffer(), " for transferring the texture");
		} else
			RE_FATAL_ERROR("Failed to create temporary Vulkan buffer to transfer texture to GPU");
		delete pVulkanTexture;
		return nullptr;
	}
	
	Texture alloc_texture_loading_from_file(const char *const pacPathToTextureFile) {
		PRINT_DEBUG("Loading image from \"", pacPathToTextureFile, "\"");
		uint32_t u32Width, u32Height, u32Channels;
		uint8_t *const pau8TextureBinaries = stbi_load(pacPathToTextureFile, reinterpret_cast<int32_t*>(&u32Width), reinterpret_cast<int32_t*>(&u32Height), reinterpret_cast<int32_t*>(&u32Channels), 0);
		if (pau8TextureBinaries && u32Width && u32Height && u32Channels)
			return alloc_texture_from_binary_data(pau8TextureBinaries, u32Width, u32Height, u32Channels);
		RE_FATAL_ERROR("Failed loading image file \"", pacPathToTextureFile, "\"");
		return nullptr;
	}

	void free_texture(const Texture hTexture) {
		if (!hTexture) {
			RE_NOTE("A null texture had to be freed. The engine will ignore this request");
			return;
		} else if (!vk_hDevice) {
			RE_ERROR("Textures aren't valid anymore, when the engine doesn't run, so they cannot be destroyed either");
			RE_NOTE("The engine will attempt to free some memory used for texture ", hTexture);
			delete reinterpret_cast<const VulkanTexture*>(hTexture);
			return;
		}
		PRINT_DEBUG("Freeing texture ", hTexture);
		const VulkanTexture *const pVulkanTexture = reinterpret_cast<VulkanTexture*>(hTexture);
		vkDestroyImageView(vk_hDevice, pVulkanTexture->vk_hImageView, nullptr);
		vkDestroyImage(vk_hDevice, pVulkanTexture->vk_hImage, nullptr);
		vkFreeMemory(vk_hDevice, pVulkanTexture->vk_hImageMemory, nullptr);
		delete pVulkanTexture;
#ifndef RE_DISABLE_PRINT_DEBUGS
		for (VulkanSprite *const pSprite : vulkanSprites)
			if (pSprite->pTexture == pVulkanTexture)
				RE_WARNING("There might be a dangling pointer in sprite ", reinterpret_cast<Sprite>(pSprite), " after texture ", hTexture, " (", pVulkanTexture->a2u32Size[0], "x", pVulkanTexture->a2u32Size[1], ") has been freed! Destroy the sprite or assign it a valid texture to avoid undefined behaviour or severe crashes");
#endif
	}

	[[nodiscard]]
	uint32_t get_width(const Texture hTexture) {
		if (!hTexture) {
			RE_ERROR("A null texture has been passed to get its width");
			return 0;
		}
		PRINT_DEBUG("Reading and returning width of texture ", hTexture);
		return reinterpret_cast<VulkanTexture*>(hTexture)->a2u32Size[0];
	}

	[[nodiscard]]
	uint32_t get_height(const Texture hTexture) {
		if (!hTexture) {
			RE_ERROR("A null texture has been passed to get its height");
			return 0;
		}
		PRINT_DEBUG("Reading and returning height of texture ", hTexture);
		return reinterpret_cast<VulkanTexture*>(hTexture)->a2u32Size[1];
	}

	void get_extent(const Texture hTexture, uint32_t *const pa2u32Extent) {
		if (!hTexture) {
			RE_ERROR("A null texture has been passed to get its extent");
			return;
		}
		PRINT_DEBUG("Reading and writing extent of texture ", hTexture, " to array pointer ", pa2u32Extent);
		std::memcpy(pa2u32Extent, &reinterpret_cast<const VulkanTexture*>(hTexture)->a2u32Size, sizeof(uint32_t) * 2);
	}

}
