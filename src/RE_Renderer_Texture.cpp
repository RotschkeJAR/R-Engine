#include "RE_Renderer_Texture.hpp"
#include "RE_Main.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#define STBI_ASSERT(x)
#include "stb_image.h"

namespace RE {

	typedef uint8_t VulkanTextureFormatsSupportedFlags;
	enum VulkanTextureFormatsSupportedFlagBit : VulkanTextureFormatsSupportedFlags {
		TEXTURE_FORMAT_SUPPORTED_R_BIT = 0x1,
		TEXTURE_FORMAT_SUPPORTED_RG_BIT = 0x2,
		TEXTURE_FORMAT_SUPPORTED_RGB_BIT = 0x4
	};

	static std::jthread nextTextureIndexSearchThread;
	static std::unique_ptr<VulkanTexture[]> vulkanTextures;
	static uint32_t u32MaxTextureExtent = 1920;
	static uint16_t u16MaxTextureCount = 1024,
		u16CurrentTextureCount = 0,
		u16NextTextureIndex = 0;
	static VulkanTextureFormatsSupportedFlags eTextureFormatsSupported;

	template <VulkanTextureFormatsSupportedFlagBit... eTextureFormatBits>
	inline bool are_texture_formats_supported() {
		constexpr VulkanTextureFormatsSupportedFlags eTextureFormats = (eTextureFormatBits | ...);
		return (eTextureFormatsSupported & eTextureFormats) == eTextureFormats;
	}

	bool init_renderer_textures() {
		if (u16MaxTextureCount)
			vulkanTextures = std::make_unique<VulkanTexture[]>(u16MaxTextureCount);
		eTextureFormatsSupported = 0;
		return true;
	}

	void destroy_renderer_textures() {
		vulkanTextures.reset();
		u16CurrentTextureCount = 0;
		if (nextTextureIndexSearchThread.joinable())
			nextTextureIndexSearchThread.join();
		u16NextTextureIndex = 0;
	}

	uint16_t get_index_of_texture(const VulkanTexture *const pTexture) {
		return static_cast<uint16_t>(pTexture - vulkanTextures.get());
	}

	static void search_next_texture_index(uint16_t u16Expiry) {
		while (true) {
			u16NextTextureIndex = (u16NextTextureIndex + 1) % u16MaxTextureCount;
			if (!vulkanTextures[u16NextTextureIndex].imageMemory.valid())
				break;
			else if (!u16Expiry)
				RE_ABORT("Search for the next free slot for texture allocation has expired");
			u16Expiry--;
		}
	}

	[[nodiscard]]
	Texture alloc_texture_from_binary_data(const uint8_t *const pau8TextureBinaries, const uint32_t u32Width, const uint32_t u32Height, const uint32_t u32Channels) {
		if (u16CurrentTextureCount == u16MaxTextureCount)
			RE_ABORT("Textures overallocated. Maximum was ", u16MaxTextureCount);
		else if (!pau8TextureBinaries || !u32Width || !u32Height || !u32Channels) {
			RE_ERROR("Textures cannot be allocated, when its binaries are null or width, height or number of channels are zero");
			return nullptr;
		} else if (!bRunning) {
			RE_ERROR("Textures cannot be created, when the engine is not running");
			return nullptr;
		} else if (u32Width > u32MaxTextureExtent || u32Height > u32MaxTextureExtent) {
			RE_FATAL_ERROR("The extent of the texture (", u32Width, ", ", u32Height, ") goes beyond the limit (", u32MaxTextureExtent, ")");
			return nullptr;
		}
		if (nextTextureIndexSearchThread.joinable()) {
			PRINT_DEBUG("Waiting for search for the next free texture slot to finish");
			nextTextureIndexSearchThread.join();
		}
		PRINT_DEBUG("Allocating new texture from heap");
		VulkanTexture *const pVulkanTexture = std::addressof(vulkanTextures[u16NextTextureIndex]);
		pVulkanTexture->u32Count = 1;
		uint32_t u32ActualChannels;
		switch (u32Channels) {
			case 1:
				if (are_texture_formats_supported<TEXTURE_FORMAT_SUPPORTED_R_BIT>()) {
					pVulkanTexture->vk_eFormat = VK_FORMAT_R8_SRGB;
					u32ActualChannels = 1;
					break;
				}
				[[fallthrough]];
			case 2:
				if (are_texture_formats_supported<TEXTURE_FORMAT_SUPPORTED_RG_BIT>()) {
					pVulkanTexture->vk_eFormat = VK_FORMAT_R8G8_SRGB;
					u32ActualChannels = 2;
					break;
				}
				[[fallthrough]];
			case 3:
				if (are_texture_formats_supported<TEXTURE_FORMAT_SUPPORTED_RGB_BIT>()) {
					pVulkanTexture->vk_eFormat = VK_FORMAT_R8G8B8_SRGB;
					u32ActualChannels = 3;
					break;
				}
				[[fallthrough]];
			case 4:
				pVulkanTexture->vk_eFormat = VK_FORMAT_R8G8B8A8_SRGB;
				u32ActualChannels = 4;
				break;
			[[unlikely]] default:
				RE_ERROR("The texture contains supposedly ", u32Channels, " channels, which is unknown to the engine. Therefore the texture won't be allocated");
				return nullptr;
		}
		PRINT_DEBUG("Creating temporary staging Vulkan buffer for transferring texture binaries");
		const VkDeviceSize vk_imageBufferSize = sizeof(uint8_t) * u32Width * u32Height * u32ActualChannels;
		Vulkan_Buffer stagingImageBuffer(0, vk_imageBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 1, nullptr, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		if (stagingImageBuffer.valid()) {
			uint8_t *pau8StagingBufferContent;
			if (stagingImageBuffer.get_memory().map(0, 0, VK_WHOLE_SIZE, reinterpret_cast<void**>(&pau8StagingBufferContent))) {
				std::jthread textureCopyThread([&]() {
							if (u32Channels == u32ActualChannels)
								std::memcpy(pau8StagingBufferContent, pau8TextureBinaries, vk_imageBufferSize);
							else {
								uint8_t a4u8ChannelValue[4];
								for (uint64_t u64PixelIndex = 0; u64PixelIndex < u32Width * u32Height; u64PixelIndex++) {
									switch (u32Channels) {
										case 1:
											a4u8ChannelValue[0] = pau8TextureBinaries[u64PixelIndex * u32Channels + 0];
											a4u8ChannelValue[1] = pau8TextureBinaries[u64PixelIndex * u32Channels + 0];
											a4u8ChannelValue[2] = pau8TextureBinaries[u64PixelIndex * u32Channels + 0];
											a4u8ChannelValue[3] = 1.0f;
											break;
										case 2:
											a4u8ChannelValue[0] = pau8TextureBinaries[u64PixelIndex * u32Channels + 0];
											a4u8ChannelValue[1] = pau8TextureBinaries[u64PixelIndex * u32Channels + 0];
											a4u8ChannelValue[2] = pau8TextureBinaries[u64PixelIndex * u32Channels + 0];
											a4u8ChannelValue[3] = pau8TextureBinaries[u64PixelIndex * u32Channels + 1];
											break;
										case 3:
											a4u8ChannelValue[0] = pau8TextureBinaries[u64PixelIndex * u32Channels + 0];
											a4u8ChannelValue[1] = pau8TextureBinaries[u64PixelIndex * u32Channels + 1];
											a4u8ChannelValue[2] = pau8TextureBinaries[u64PixelIndex * u32Channels + 2];
											a4u8ChannelValue[3] = 1.0f;
											break;
									}
									switch (u32ActualChannels) {
										case 2:
											pau8StagingBufferContent[u64PixelIndex * u32ActualChannels + 0] = a4u8ChannelValue[0];
											pau8StagingBufferContent[u64PixelIndex * u32ActualChannels + 1] = a4u8ChannelValue[3];
											break;
										case 3:
											pau8StagingBufferContent[u64PixelIndex * u32ActualChannels + 0] = a4u8ChannelValue[0];
											pau8StagingBufferContent[u64PixelIndex * u32ActualChannels + 1] = a4u8ChannelValue[1];
											pau8StagingBufferContent[u64PixelIndex * u32ActualChannels + 2] = a4u8ChannelValue[2];
											break;
										case 4:
											pau8StagingBufferContent[u64PixelIndex * u32ActualChannels + 0] = a4u8ChannelValue[0];
											pau8StagingBufferContent[u64PixelIndex * u32ActualChannels + 1] = a4u8ChannelValue[1];
											pau8StagingBufferContent[u64PixelIndex * u32ActualChannels + 2] = a4u8ChannelValue[2];
											pau8StagingBufferContent[u64PixelIndex * u32ActualChannels + 3] = a4u8ChannelValue[3];
											break;
									}
								}
							}
						});
				PRINT_DEBUG("Creating Vulkan image for storing texture on GPU");
				if (create_vulkan_image(0,
						VK_IMAGE_TYPE_2D,
						pVulkanTexture->vk_eFormat,
						VkExtent3D{u32Width, u32Height, 1},
						1,
						1,
						VK_SAMPLE_COUNT_1_BIT,
						VK_IMAGE_TILING_OPTIMAL,
						VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
						1,
						nullptr,
						VK_IMAGE_LAYOUT_UNDEFINED,
						&pVulkanTexture->vk_hImage)) {
					if (pVulkanTexture->imageMemory.alloc_for_image(pVulkanTexture->vk_hImage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_SUCCESS) {
						PRINT_DEBUG("Assigning commands and recording Vulkan task for transferring texture to GPU");
						const uint8_t a2u8LogicalQueueIndices[2] = {RE_VK_LOGICAL_QUEUE_IGNORED, renderTasks[0].logical_queue_index_for_function(RENDER_TASK_SUBINDEX_RENDERING)};
						constexpr VkQueueFlagBits vk_a2eQueueTypes[2] = {VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT};
						constexpr uint32_t a2u32SeparationIds[2] = {0, 1};
						const VulkanTask_Queues occupiedTransferQueues = {
							.pau8LogicalQueueIndices = a2u8LogicalQueueIndices,
							.vk_paeQueueTypes = vk_a2eQueueTypes,
							.pau32StrictSeparationIds = a2u32SeparationIds,
							.u32FunctionsCount = 2
						};
						VulkanTask transferTask(occupiedTransferQueues, false, false, true);
						transferTask.record(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
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
									.srcBuffer = stagingImageBuffer.get(),
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
						});
						transferTask.record(1, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
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
						});
						PRINT_DEBUG("Submitting Vulkan task for transferring texture to GPU");
						Vulkan_Fence transferFence(0);
						textureCopyThread.join();
						constexpr VkPipelineStageFlags2 vk_eWaitStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
						transferTask.submit(0, nullptr, &vk_eWaitStage, 0, nullptr, transferFence.get());
						VkComponentMapping vk_channelMapping;
						switch (pVulkanTexture->vk_eFormat) {
							case VK_FORMAT_R8_SRGB:
								vk_channelMapping.r = VK_COMPONENT_SWIZZLE_R;
								vk_channelMapping.g = VK_COMPONENT_SWIZZLE_R;
								vk_channelMapping.b = VK_COMPONENT_SWIZZLE_R;
								vk_channelMapping.a = VK_COMPONENT_SWIZZLE_ONE;
								break;
							case VK_FORMAT_R8G8_SRGB:
								vk_channelMapping.r = VK_COMPONENT_SWIZZLE_R;
								vk_channelMapping.g = VK_COMPONENT_SWIZZLE_R;
								vk_channelMapping.b = VK_COMPONENT_SWIZZLE_R;
								vk_channelMapping.a = VK_COMPONENT_SWIZZLE_G;
								break;
							case VK_FORMAT_R8G8B8_SRGB:
								vk_channelMapping.r = VK_COMPONENT_SWIZZLE_IDENTITY;
								vk_channelMapping.g = VK_COMPONENT_SWIZZLE_IDENTITY;
								vk_channelMapping.b = VK_COMPONENT_SWIZZLE_IDENTITY;
								vk_channelMapping.a = VK_COMPONENT_SWIZZLE_ONE;
								break;
							case VK_FORMAT_R8G8B8A8_SRGB:
								vk_channelMapping.r = VK_COMPONENT_SWIZZLE_IDENTITY;
								vk_channelMapping.g = VK_COMPONENT_SWIZZLE_IDENTITY;
								vk_channelMapping.b = VK_COMPONENT_SWIZZLE_IDENTITY;
								vk_channelMapping.a = VK_COMPONENT_SWIZZLE_IDENTITY;
								break;
							[[unlikely]] default:
								RE_ABORT("Unknown format selected for the texture: ", std::hex, pVulkanTexture->vk_eFormat, " (likely due to corruption)");
						}
						if (create_vulkan_image_view(0,
								pVulkanTexture->vk_hImage,
								VK_IMAGE_VIEW_TYPE_2D_ARRAY,
								pVulkanTexture->vk_eFormat,
								vk_channelMapping,
								VkImageSubresourceRange {
									.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
									.baseMipLevel = 0,
									.levelCount = 1,
									.baseArrayLayer = 0,
									.layerCount = 1
								},
								&pVulkanTexture->vk_hImageView)) {
							pVulkanTexture->a2u32Size[0] = u32Width;
							pVulkanTexture->a2u32Size[1] = u32Height;
							u16CurrentTextureCount++;
							if (u16CurrentTextureCount < u16MaxTextureCount)
								nextTextureIndexSearchThread = std::jthread(search_next_texture_index, u16MaxTextureCount);
							VkDescriptorImageInfo vk_imageInfo;
							vk_imageInfo.imageView = pVulkanTexture->vk_hImageView;
							vk_imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
							const VkWriteDescriptorSet vk_updateInfo = {
								.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
								.pNext = nullptr,
								.dstSet = vk_hTextureDescSet,
								.dstBinding = RE_VK_SPRITE_DESC_SET_TEXTURE_BINDING_INDEX,
								.dstArrayElement = get_index_of_texture(pVulkanTexture),
								.descriptorCount = 1,
								.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
								.pImageInfo = &vk_imageInfo,
								.pBufferInfo = nullptr,
								.pTexelBufferView = nullptr
							};
							transferFence.wait_for();
							if (!are_vulkan_features_enabled<ENABLED_FEATURE_UPDATE_UNUSED_DESCRIPTORS_WHILE_PENDING_BIT>())
								wait_for_rendering_finished();
							vkUpdateDescriptorSets(vk_hDevice, 1, &vk_updateInfo, 0, nullptr);
							return reinterpret_cast<Texture>(pVulkanTexture);
						} else
							RE_FATAL_ERROR("Failed to create Vulkan image view for image ", pVulkanTexture->vk_hImage);
						PRINT_DEBUG("Freeing Vulkan image's memory on GPU due to failure creating an image view");
						transferFence.wait_for();
						pVulkanTexture->imageMemory.free();
					} else
						RE_FATAL_ERROR("Failed to allocate Vulkan memory for the image ", std::hex, pVulkanTexture->vk_hImage, " to store texture data");
					PRINT_DEBUG("Destroying Vulkan image ", pVulkanTexture->vk_hImage, " due to failure allocating essential resources for the texture on the GPU");
					vkDestroyImage(vk_hDevice, pVulkanTexture->vk_hImage, nullptr);
				} else
					RE_FATAL_ERROR("Failed to create Vulkan image on GPU to permanently store the texture");
			} else
				RE_FATAL_ERROR("Failed to map temporary Vulkan buffer for transferring the texture");
		} else
			RE_FATAL_ERROR("Failed to create temporary Vulkan buffer to transfer texture to GPU");
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
		if (!hTexture)
			RE_ERROR("A null texture had to be freed. The engine will ignore this request");
		else if (bRunning) {
			PRINT_DEBUG("Freeing texture ", hTexture);
			VulkanTexture *const pVulkanTexture = reinterpret_cast<VulkanTexture*>(hTexture);
			vkDestroyImageView(vk_hDevice, pVulkanTexture->vk_hImageView, nullptr);
			vkDestroyImage(vk_hDevice, pVulkanTexture->vk_hImage, nullptr);
			pVulkanTexture->imageMemory.free();
			u16CurrentTextureCount--;
		} else
			RE_ERROR("Textures aren't valid anymore, when the engine doesn't run, so they cannot be destroyed either");
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

	void set_max_texture_count(const uint16_t u16NewMaxTextureCount) {
	    if (!bRunning)
			RE_ERROR("The maximum texture count cannot be changed while the engine is running");
	    else
			u16MaxTextureCount = u16NewMaxTextureCount;
	}

	[[nodiscard]]
	uint16_t get_max_texture_count() {
	    return u16MaxTextureCount;
	}

	[[nodiscard]]
	uint16_t get_current_texture_count() {
	    return u16CurrentTextureCount;
	}

	[[nodiscard]]
	uint16_t get_remaining_texture_allocs() {
	    return u16MaxTextureCount - u16CurrentTextureCount;
	}

	[[nodiscard]]
	uint32_t get_max_texture_extent() {
		return u32MaxTextureExtent;
	}

	void set_max_texture_extent(const uint32_t u32NewMaxTextureExtent) {
		u32MaxTextureExtent = u32NewMaxTextureExtent;
	}

}
