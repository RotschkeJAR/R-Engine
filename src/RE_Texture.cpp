#include "RE_Texture.hpp"
#include "RE_Vulkan_Device.hpp"
#include "RE_GPU.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_List_GameObject.hpp"
#include "RE_Renderer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#define STBI_ASSERT(x)
#include "stb_image.h"

namespace RE {

	Texture alloc_texture_from_binary_data(const uint8_t *const pau8TextureBinaries, const uint32_t u32Width, const uint32_t u32Height, const uint32_t u32Channels) {
		if (!pau8TextureBinaries || !u32Width || !u32Height || !u32Channels) {
			RE_ERROR("Textures cannot be allocated, when its binaries are null or width, height or number of channels are zero");
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
				if (create_vulkan_image(0, VK_IMAGE_TYPE_2D, pVulkanTexture->vk_eFormat, VkExtent3D{}, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 1, nullptr, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &pVulkanTexture->vk_hImage, &pVulkanTexture->vk_hImageMemory)) {
					PRINT_DEBUG("Creating temporary Vulkan fence to wait for finishing texture creation on GPU");
					Vulkan_Fence textureTransferFence;
					if (textureTransferFence.is_valid()) {
						PRINT_DEBUG("Creating Vulkan task for transferring texture to GPU");
						constexpr uint32_t u32TaskFunctions = 2;
						const uint8_t au8LogicalQueues[u32TaskFunctions] = {RE_VK_LOGICAL_QUEUE_IGNORED, get_render_graphics_queue_logical_index()};
						constexpr VkQueueFlagBits vk_aeRequiredQueueType[u32TaskFunctions] = {VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT};
						constexpr uint32_t au32SeparationIds[u32TaskFunctions] = {0, 0};
						const VulkanTask_Queues requiredQueues = {
							.pau8LogicalQueueIndices = au8LogicalQueues,
							.vk_paeQueueTypes = vk_aeRequiredQueueType,
							.pau32StrictSeparationIds = au32SeparationIds,
							.u32FunctionsCount = u32TaskFunctions
						};
						VulkanTask textureTransferTask(requiredQueues, true);
						if (textureTransferTask.is_valid()) {
							PRINT_DEBUG("Assigning commands and recording Vulkan task for transferring texture to GPU");
							textureTransferTask.paFunctions[0] = [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t 	u8NextLogicalQueue) {
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
								const VkImageMemoryBarrier2 vk_imageLayoutFinalBarrier = {
									.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
									.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
									.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
									.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
									.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT,
									.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
									.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
									.srcQueueFamilyIndex = queueFamilyIndices[u8CurrentLogicalQueue],
									.dstQueueFamilyIndex = queueFamilyIndices[u8NextLogicalQueue],
									.image = pVulkanTexture->vk_hImage,
									.subresourceRange = {
										.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
										.baseMipLevel = 0,
										.levelCount = 1,
										.baseArrayLayer = 0,
										.layerCount = 1
									}
								};
								const VkDependencyInfo vk_transitImageLayoutFinalInfo = {
									.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
									.imageMemoryBarrierCount = 1,
									.pImageMemoryBarriers = &vk_imageLayoutFinalBarrier
								};
								vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_transitImageLayoutFinalInfo);
							};
							textureTransferTask.paFunctions[1] = [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
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
							textureTransferTask.record(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
							PRINT_DEBUG("Submitting Vulkan task for transferring texture to GPU");
							textureCopyThread.join();
							if (textureTransferTask.submit(0, nullptr, 0, nullptr, textureTransferFence.get_fence())) {
								PRINT_DEBUG("Creating Vulkan image view for Vulkan image ", pVulkanTexture->vk_hImage, " containing texture");
								if (create_vulkan_image_view(pVulkanTexture->vk_hImage, VK_IMAGE_VIEW_TYPE_2D, pVulkanTexture->vk_eFormat, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1, &pVulkanTexture->vk_hImageView)) {
									PRINT_DEBUG("Waiting for transferring texture to GPU");
									pVulkanTexture->u32Channels = u32Channels;
									pVulkanTexture->a2u32Size[0] = u32Width;
									pVulkanTexture->a2u32Size[1] = u32Height;
									textureTransferFence.wait_for();
									return reinterpret_cast<Texture>(pVulkanTexture);
								}
								PRINT_DEBUG("Waiting for transferring texture to GPU");
								textureTransferFence.wait_for();
							} else
								RE_FATAL_ERROR("Failed submitting Vulkan task to transfer texture to GPU");
						} else
							RE_FATAL_ERROR("Failed to initialize Vulkan task for transferring texture to GPU");
					} else
						RE_FATAL_ERROR("Failed to create Vulkan fence to wait for texture transferring to GPU");
					PRINT_DEBUG("Destroying Vulkan image ", pVulkanTexture->vk_hImage, " and freeing its memory ", pVulkanTexture->vk_hImageMemory, " on GPU due to failure allocating essential resources for the texture");
					vkDestroyImage(vk_hDevice, pVulkanTexture->vk_hImage, nullptr);
					vkFreeMemory(vk_hDevice, pVulkanTexture->vk_hImageMemory, nullptr);
				} else
					RE_FATAL_ERROR("Failed to create Vulkan image on GPU to permanently store the texture");
			}
		} else
			RE_FATAL_ERROR("Failed to create temporary Vulkan buffer to transfer texture to GPU");
		delete pVulkanTexture;
		return nullptr;
	}
	
	Texture alloc_texture_loading_from_file(const char *const pacPathToTextureFile) {
		PRINT_DEBUG("Loading image from \"", pacPathToTextureFile, "\"");
		return nullptr;
	}

	void free_texture(const Texture hTexture) {
		if (!hTexture) {
			RE_NOTE("A null texture had to be freed. The engine will ignore this request");
			return;
		}
		PRINT_DEBUG("Freeing texture ", hTexture);
		const VulkanTexture *const pVulkanTexture = reinterpret_cast<VulkanTexture*>(hTexture);
		vkDestroyImageView(vk_hDevice, pVulkanTexture->vk_hImageView, nullptr);
		vkDestroyImage(vk_hDevice, pVulkanTexture->vk_hImage, nullptr);
		vkFreeMemory(vk_hDevice, pVulkanTexture->vk_hImageMemory, nullptr);
		delete pVulkanTexture;
	}

	void free_texture_and_fix_dangling_pointers(const Texture hTexture) {
		PRINT_DEBUG("Freeing texture ", hTexture, " and removing dangling pointers pointing to it");
		free_texture(hTexture);
	}

}
