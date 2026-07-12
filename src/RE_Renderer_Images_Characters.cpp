#include "RE_Renderer_Images_Internal.hpp"
#include "RE_Window.hpp"
#include "RE_Asset.hpp"

namespace RE {
	
	VkImage vk_hCharacterImage;
	VkImageView vk_hCharacterImageView;

	bool create_character_image() {
		PRINT_DEBUG("Creating Vulkan image for containing characters");
		if (create_vulkan_image(
				0,
				VK_IMAGE_TYPE_2D,
				VK_FORMAT_R8_UNORM,
				VkExtent3D {
					.width = CHAR_TEXTURE_SIZE,
					.height = CHAR_TEXTURE_SIZE,
					.depth = 1
				},
				1,
				CHAR_TEXTURE_COUNT,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				1,
				nullptr,
				VK_IMAGE_LAYOUT_UNDEFINED,
				&vk_hCharacterImage))
			return true;
		else
			RE_FATAL_ERROR("Failed to create Vulkan image containing characters");
		return false;
	}

	bool create_character_image_view() {
		PRINT_DEBUG("Creating staging Vulkan buffer for transferring character textures to GPU");
		constexpr VkDeviceSize vk_stagingBufferSize = (CHAR_TEXTURE_SIZE * CHAR_TEXTURE_SIZE) * CHAR_TEXTURE_COUNT * sizeof(uint8_t);
		Vulkan_Buffer stagingBuffer(0, vk_stagingBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 1, nullptr, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		if (stagingBuffer.valid()) {
			PRINT_DEBUG("Mapping staging Vulkan buffer to CPU");
			void *pStagingBufferMemory;
			if (stagingBuffer.get_memory().map(0, 0, VK_WHOLE_SIZE, &pStagingBufferMemory)) {
				PRINT_DEBUG("Opening asset file containing characters");
				AssetImage charactersAsset;
				if (asset_open("rsc/characters.png", charactersAsset.file)) {
					PRINT_DEBUG("Loading characters from content of asset file");
					if (asset_image_load(charactersAsset, 1)) {
						PRINT_DEBUG("Closing asset file containing characters");
						asset_close(charactersAsset.file);
						PRINT_DEBUG("Copying characters to staging Vulkan buffer's memory");
						std::memcpy(pStagingBufferMemory, charactersAsset.pBuffer, vk_stagingBufferSize);
						asset_image_free(charactersAsset);
						stagingBuffer.get_memory().flush_mapped_memory(0, VK_WHOLE_SIZE);
						PRINT_DEBUG("Creating Vulkan task for transferring characters to GPU");
						constexpr uint32_t u32FunctionCount = 2;
						const uint8_t au8QueueIndices[u32FunctionCount] = {RE_VK_LOGICAL_QUEUE_IGNORED, aRenderTasks[0].logical_queue_index_for_function(RENDER_TASK_SUBINDEX_RENDERING)};
						constexpr VkQueueFlagBits vk_aeQueueTypes[u32FunctionCount] = {VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT};
						constexpr uint32_t au32SeparationIds[u32FunctionCount] = {0, 1};
						const VulkanTask_Queues queueInfo = {
							.pau8LogicalQueueIndices = au8QueueIndices,
							.vk_paeQueueTypes = vk_aeQueueTypes,
							.pau32StrictSeparationIds = au32SeparationIds,
							.u32FunctionsCount = u32FunctionCount
						};
						VulkanTask transferTask(queueInfo, false, false, true);
						if (transferTask.valid()) {
							PRINT_DEBUG("Recording first function of transfer task");
							if (transferTask.record(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, [&](VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue) {
										const VkImageMemoryBarrier vk_imageBarrier0 = {
											.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
											.pNext = nullptr,
											.srcAccessMask = VK_ACCESS_NONE,
											.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
											.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
											.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
											.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
											.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
											.image = vk_hCharacterImage,
											.subresourceRange = {
												.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
												.baseMipLevel = 0,
												.levelCount = 1,
												.baseArrayLayer = 0,
												.layerCount = CHAR_TEXTURE_COUNT
											}
										};
										vkCmdPipelineBarrier(vk_hCommandBuffer, VK_PIPELINE_STAGE_NONE, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &vk_imageBarrier0);
										constexpr VkBufferImageCopy vk_aImageCopies[] = {
											{
												.bufferOffset = 0,
												.bufferRowLength = 0,
												.bufferImageHeight = 0,
												.imageSubresource = {
													.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
													.mipLevel = 0,
													.baseArrayLayer = 0,
													.layerCount = CHAR_TEXTURE_COUNT
												},
												.imageOffset = {},
												.imageExtent = {
													.width = CHAR_TEXTURE_SIZE,
													.height = CHAR_TEXTURE_SIZE,
													.depth = 1
												}
											}
										};
										vkCmdCopyBufferToImage(vk_hCommandBuffer, stagingBuffer(), vk_hCharacterImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, sizeof(vk_aImageCopies) / sizeof(vk_aImageCopies[0]), vk_aImageCopies);
										const VkImageMemoryBarrier vk_imageBarrier1 = {
											.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
											.pNext = nullptr,
											.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
											.dstAccessMask = VK_ACCESS_NONE,
											.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
											.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
											.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
											.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
											.image = vk_hCharacterImage,
											.subresourceRange = {
												.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
												.baseMipLevel = 0,
												.levelCount = 1,
												.baseArrayLayer = 0,
												.layerCount = CHAR_TEXTURE_COUNT
											}
										};
										vkCmdPipelineBarrier(vk_hCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &vk_imageBarrier1);
										if (u8CurrentLogicalQueue == u8NextLogicalQueue)
											return;
										const VkImageMemoryBarrier vk_imageBarrier2 = {
											.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
											.pNext = nullptr,
											.srcAccessMask = VK_ACCESS_NONE,
											.dstAccessMask = VK_ACCESS_NONE,
											.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
											.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
											.srcQueueFamilyIndex = queueFamilyIndices[u8CurrentLogicalQueue],
											.dstQueueFamilyIndex = queueFamilyIndices[u8NextLogicalQueue],
											.image = vk_hCharacterImage,
											.subresourceRange = {
												.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
												.baseMipLevel = 0,
												.levelCount = 1,
												.baseArrayLayer = 0,
												.layerCount = CHAR_TEXTURE_COUNT
											}
										};
										vkCmdPipelineBarrier(vk_hCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &vk_imageBarrier2);
									})) {
								PRINT_DEBUG("Recording second function of transfer task");
								if (transferTask.record(1, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, [&](VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue) {
											if (u8PreviousLogicalQueue == u8CurrentLogicalQueue)
												return;
											const VkImageMemoryBarrier vk_imageBarrier0 = {
												.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
												.pNext = nullptr,
												.srcAccessMask = VK_ACCESS_NONE,
												.dstAccessMask = VK_ACCESS_NONE,
												.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
												.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
												.srcQueueFamilyIndex = queueFamilyIndices[u8PreviousLogicalQueue],
												.dstQueueFamilyIndex = queueFamilyIndices[u8CurrentLogicalQueue],
												.image = vk_hCharacterImage,
												.subresourceRange = {
													.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
													.baseMipLevel = 0,
													.levelCount = 1,
													.baseArrayLayer = 0,
													.layerCount = CHAR_TEXTURE_COUNT
												}
											};
											vkCmdPipelineBarrier(vk_hCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &vk_imageBarrier0);
										})) {
									PRINT_DEBUG("Creating Vulkan fence for waiting for the transfer to finish");
									Vulkan_Fence transferFence(0);
									if (transferFence.valid()) {
										PRINT_DEBUG("Submitting transfer task to the GPU");
										constexpr VkPipelineStageFlags2 vk_aeInternWaitStages[] = {
											VK_PIPELINE_STAGE_2_TRANSFER_BIT | VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT
										};
										if (transferTask.submit(
												0,
												nullptr,
												vk_aeInternWaitStages,
												0,
												nullptr,
												transferFence())) {
											PRINT_DEBUG("Creating Vulkan image view of image ", vk_hCharacterImage, " containing characters");
											if (create_vulkan_image_view(
													0,
													vk_hCharacterImage,
													VK_IMAGE_VIEW_TYPE_2D_ARRAY,
													VK_FORMAT_R8_UNORM,
													VkComponentMapping {
														.r = VK_COMPONENT_SWIZZLE_ONE,
														.g = VK_COMPONENT_SWIZZLE_ONE,
														.b = VK_COMPONENT_SWIZZLE_ONE,
														.a = VK_COMPONENT_SWIZZLE_R
													},
													VkImageSubresourceRange {
														.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
														.baseMipLevel = 0,
														.levelCount = 1,
														.baseArrayLayer = 0,
														.layerCount = CHAR_TEXTURE_COUNT
													},
													&vk_hCharacterImageView)) {
												PRINT_DEBUG("Waiting for transfer task to finish");
												transferFence.wait_for(std::numeric_limits<uint64_t>::max());
												return true;
											} else
												RE_FATAL_ERROR("Failed creating a Vulkan image view pointing to the characters");
											PRINT_DEBUG("Waiting for transfer task to finish for failing to create Vulkan image view of image ", vk_hCharacterImage);
											transferFence.wait_for(std::numeric_limits<uint64_t>::max());
										} else
											RE_FATAL_ERROR("Failed submitting the Vulkan task to the GPU to transfer characters");
									} else
										RE_FATAL_ERROR("Failed creating a Vulkan fence to wait for finishing transferring characters to the GPU");
								} else
									RE_FATAL_ERROR("Failed recording the 2nd scope of the Vulkan task for transferring characters to the GPU");
							} else
								RE_FATAL_ERROR("Failed recording the 1st scope of the Vulkan task for transferring characters to the GPU");
						} else
							RE_FATAL_ERROR("Failed creating a Vulkan task for transferring the staging buffer to the image containing characters");
					} else {
						RE_FATAL_ERROR("Failed loading characters from asset file");
						PRINT_DEBUG("Closing asset file containing characters due to failure loading them");
						asset_close(charactersAsset.file);
					}
				} else
					RE_FATAL_ERROR("Failed opening asset file containing characters");
			} else
				RE_FATAL_ERROR("Failed mapping memory of staging Vulkan buffer to RAM for the characters");
		} else
			RE_FATAL_ERROR("Failed creating a staging Vulkan buffer for the characters");
		return false;
	}

	void destroy_character_image() {
		PRINT_DEBUG("Destroying Vulkan image ", vk_hCharacterImage, " containing characters");
		vkDestroyImage(vk_hDevice, vk_hCharacterImage, nullptr);
	}

	void destroy_character_image_view() {
		PRINT_DEBUG("Destroying Vulkan image view ", vk_hCharacterImageView, " used for characters");
		vkDestroyImageView(vk_hDevice, vk_hCharacterImageView, nullptr);
	}

}
