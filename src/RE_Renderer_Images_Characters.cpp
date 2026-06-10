#include "RE_Renderer_Images_Internal.hpp"

namespace RE {

#define CHAR_TEXTURE_SIZE 8

	constexpr uint8_t aau8CharacterTextureData[][CHAR_TEXTURE_SIZE * CHAR_TEXTURE_SIZE] = {
		{
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
		}, {
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
		}
	};
	
	VkImage vk_hCharacterImage;
	VkImageView vk_hCharacterImageView;

	bool create_character_image() {
		PRINT_DEBUG("");
		if (create_vulkan_image(VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT,
				VK_IMAGE_TYPE_2D,
				VK_FORMAT_R8_UNORM,
				VkExtent3D {
					.width = CHAR_TEXTURE_SIZE,
					.height = CHAR_TEXTURE_SIZE,
					.depth = 1
				},
				1,
				/*layer count*/,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				1,
				nullptr,
				VK_IMAGE_LAYOUT_UNDEFINED,
				&vk_hCharacterImage))
			return true;
		else
			RE_FATAL_ERROR("");
		return false;
	}

	bool create_character_image_view() {
		PRINT_DEBUG("");
		constexpr VkDeviceSize vk_stagingBufferSize = sizeof(uint8_t) * CHAR_TEXTURE_SIZE * CHAR_TEXTURE_SIZE * ;
		Vulkan_Buffer stagingBuffer(0, vk_stagingBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 1, nullptr, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		if (stagingBuffer.valid()) {
			PRINT_DEBUG("");
			uint8_t *pau8StagingBufferMemory;
			if (stagingBuffer.get_memory().map(0, 0, VK_WHOLE_SIZE, reinterpret_cast<void**>(pau8StagingBufferMemory))) {
				PRINT_DEBUG("");
				std::jthread stagingBufferInitThread([&]() {
							std::memset(pau8StagingBufferMemory, 0, vk_stagingBufferSize);
							std::memcpy(pau8StagingBufferMemory, aau8CharacterTextureData, vk_stagingBufferSize);
						});
				PRINT_DEBUG("");
				const uint8_t a2u8QueueIndices[2] = {RE_VK_LOGICAL_QUEUE_IGNORED, renderTask[0].logical_queue_index_for_function()};
				constexpr VkQueueFlagBits vk_a2eQueueTypes[2] = {VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT};
				constexpr uint32_t a2u32SeparationIds[2] = {0, 1};
				const VulkanTask_Queues queueInfo = {
					.pau8LogicalQueueIndices = a2u8QueueIndices,
					.vk_paeQueueTypes = vk_a2eQueueTypes,
					.pau32StrictSeparationIds = a2u32SeparationIds,
					.u32FunctionsCount = 2
				};
				VulkanTask transferTask(queueInfo, false, false, true);
				if (transferTask.valid()) {
					PRINT_DEBUG("");
					if (transferTask.record(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
								constexpr VkImageMemoryBarrier vk_imageBarrier0 = {
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
										.layerCount = 
									}
								};
								vkCmdPipelineBarrier(vk_hCommandBuffer, VK_PIPELINE_STAGE_NONE, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &vk_imageBarrier0);
								constexpr VkClearColorValue vk_imageClear = {
									.uint32 = {0U, 0U, 0U, 0U}
								};
								constexpr VkImageSubresourceRange vk_imageFill = {
									.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
									.baseMipLevel = 0,
									.levelCount = 1,
									.baseArrayLayer = 0,
									.layerCount = 
								};
								vkCmdClearColorImage(vk_hCommandBuffer, vk_hCharacterImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &vk_imageClear, 1, &vk_imageFill);
								constexpr VkBufferImageCopy vk_aImageCopies[] = {
									{
										.bufferOffset = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY,
										.bufferRowLength = 0,
										.bufferImageHeight = 0,
										.imageSubresource = {
											.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
											.mipLevel = 0,
											.baseArrayLayer = ,
											.layerCount = 
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
								constexpr VkImageMemoryBarrier vk_imageBarrier1 = {
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
										.layerCount = 
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
										.layerCount = 
									}
								};
								vkCmdPipelineBarrier(vk_hCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &vk_imageBarrier2);
							})) {
						if (transferTask.record(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
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
											.layerCount = 
										}
									};
									vkCmdPipelineBarrier(vk_hCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &vk_imageBarrier0);
								})) {
							Vulkan_Fence transferFence(0);
							if (transferFence.valid()) {
								constexpr VkPipelineStageFlags2 vk_aeInternWaitStages[] = {
									VK_PIPELINE_STAGE_2_TRANSFER_BIT | VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT
								};
								if (transferTask.submit(0, nullptr, vk_aeInternWaitStages, 0, nullptr, transferFence())) {
									if (create_vulkan_image_view(0,
											vk_hCharacterImage,
											VK_IMAGE_VIEW_TYPE_2D_ARRAY,
											VK_FORMAT_R8_UNORM,
											VkComponentMapping {
												.r = VK_COMPONENT_SWIZZLE_R,
												.g = VK_COMPONENT_SWIZZLE_R,
												.b = VK_COMPONENT_SWIZZLE_R,
												.a = VK_COMPONENT_SWIZZLE_R
											},
											VkImageSubresourceRange {
												.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
												.baseMipLevel = 0,
												.levelCount = 1,
												.baseArrayLayer = 0,
												.layerCount = 
											},
											&vk_hCharacterImageView)) {
										PRINT_DEBUG("");
										transferFence.wait_for(std::numeric_limits<uint64_t>::max());
										return true;
									} else
										RE_FATAL_ERROR("");
									PRINT_DEBUG("");
									transferFence.wait_for(std::numeric_limits<uint64_t>::max());
								} else
									RE_FATAL_ERROR("");
							} else
								RE_FATAL_ERROR("");
						} else
							RE_FATAL_ERROR("");
					} else
						RE_FATAL_ERROR("");
				} else
					RE_FATAL_ERROR("");
			} else
				RE_FATAL_ERROR("");
		} else
			RE_FATAL_ERROR("");
		return false;
	}

	void destroy_character_image() {
		PRINT_DEBUG("");
		vkDestroyImage(vk_hDevice, vk_hCharacterImage, nullptr);
	}

	void destroy_character_image_view() {
		PRINT_DEBUG("");
		vkDestroyImageView(vk_hDevice, vk_hCharacterImageView, nullptr);
	}

}
