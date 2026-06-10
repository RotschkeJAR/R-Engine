#include "RE_Renderer_Images_Internal.hpp"
#include "RE_Window.hpp"

namespace RE {
	
#ifdef RE_OS_LINUX

	VkImage vk_hWindowButtonImage;
	VkImageView vk_hWindowButtonImageView;

	bool create_window_button_image() {
		PRINT_DEBUG("Creating Vulkan image for window button textures");
		if (create_vulkan_image(0,
				VK_IMAGE_TYPE_2D,
				VK_FORMAT_R8_UNORM,
				VkExtent3D {
					.width = WINDOW_WAYLAND_BUTTON_TEXTURE_SIZE,
					.height = WINDOW_WAYLAND_BUTTON_TEXTURE_SIZE,
					.depth = 1
				},
				1,
				WINDOW_WAYLAND_BUTTON_COUNT,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				1,
				nullptr,
				VK_IMAGE_LAYOUT_UNDEFINED,
				&vk_hWindowButtonImage))
			return true;
		else
			RE_FATAL_ERROR("Failed to create Vulkan image holding window button textures");
		return false;
	}

	bool create_window_button_image_views() {
		PRINT_DEBUG("Creating Vulkan image view for window button textures");
		if (create_vulkan_image_view(0,
				vk_hWindowButtonImage,
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
					.layerCount = WINDOW_WAYLAND_BUTTON_COUNT
				},
				&vk_hWindowButtonImageView)) {
			PRINT_DEBUG("");
			Vulkan_Buffer stagingWindowButtonBuffer(0,
					sizeof(uint8_t) * WINDOW_WAYLAND_BUTTON_TEXTURE_SIZE * WINDOW_WAYLAND_BUTTON_TEXTURE_SIZE * WINDOW_WAYLAND_BUTTON_COUNT,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					1,
					nullptr,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			if (stagingWindowButtonBuffer.valid()) {
				PRINT_DEBUG("");
				uint8_t *pau8StagingBufferMemory;
				if (stagingWindowButtonBuffer.get_memory().map(0, 0, VK_WHOLE_SIZE, reinterpret_cast<void**>(&pau8StagingBufferMemory))) {
					PRINT_DEBUG("");
					constexpr uint8_t aau8TextureData[WINDOW_WAYLAND_BUTTON_COUNT][WINDOW_WAYLAND_BUTTON_TEXTURE_SIZE * WINDOW_WAYLAND_BUTTON_TEXTURE_SIZE] = {
						{
							0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0xFF, 0xFF, 0xFF,
							0,    0xFF, 0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0xFF, 0xFF, 0xFF, 0,
							0,    0,    0xFF, 0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0xFF, 0xFF, 0xFF, 0,    0,
							0,    0,    0,    0xFF, 0xFF, 0xFF, 0,    0,    0,    0,    0xFF, 0xFF, 0xFF, 0,    0,    0,
							0,    0,    0,    0,    0xFF, 0xFF, 0xFF, 0,    0,    0xFF, 0xFF, 0xFF, 0,    0,    0,    0,
							0,    0,    0,    0,    0,    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0,    0,    0,    0,    0,
							0,    0,    0,    0,    0,    0,    0xFF, 0xFF, 0xFF, 0xFF, 0,    0,    0,    0,    0,    0,
							0,    0,    0,    0,    0,    0,    0xFF, 0xFF, 0xFF, 0xFF, 0,    0,    0,    0,    0,    0,
							0,    0,    0,    0,    0,    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0,    0,    0,    0,    0,
							0,    0,    0,    0,    0xFF, 0xFF, 0xFF, 0,    0,    0xFF, 0xFF, 0xFF, 0,    0,    0,    0,
							0,    0,    0,    0xFF, 0xFF, 0xFF, 0,    0,    0,    0,    0xFF, 0xFF, 0xFF, 0,    0,    0,
							0,    0,    0xFF, 0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0xFF, 0xFF, 0xFF, 0,    0,
							0,    0xFF, 0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0xFF, 0xFF, 0xFF, 0,
							0xFF, 0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0xFF, 0xFF
						}, {
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0xFF, 0xFF,
							0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0xFF, 0xFF,
							0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0xFF, 0xFF,
							0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0xFF, 0xFF,
							0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0xFF, 0xFF,
							0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0xFF, 0xFF,
							0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0xFF, 0xFF,
							0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0xFF, 0xFF,
							0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0xFF, 0xFF,
							0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0xFF, 0xFF,
							0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
						}, {
							0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
							0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
							0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
							0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
							0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
							0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
							0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
							0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
							0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
							0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
							0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
							0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
							0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
							0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
						}
					};
					std::memcpy(pau8StagingBufferMemory, &aau8TextureData[0][0], sizeof(aau8TextureData));
					PRINT_DEBUG("");
					const uint8_t a2u8LogicalQueues[2] = {RE_VK_LOGICAL_QUEUE_IGNORED, aRenderTasks[0].logical_queue_index_for_function(RENDER_TASK_SUBINDEX_RENDERING)};
					constexpr VkQueueFlagBits vk_a2eQueueTypes[2] = {VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT};
					constexpr uint32_t a2u32SeperationIds[2] = {0, 1};
					const VulkanTask_Queues transferTaskQueues = {
						.pau8LogicalQueueIndices = a2u8LogicalQueues,
						.vk_paeQueueTypes = vk_a2eQueueTypes,
						.pau32StrictSeparationIds = a2u32SeperationIds,
						.u32FunctionsCount = 2
					};
					VulkanTask transferTask(transferTaskQueues, false, false, true);
					if (transferTask.valid()) {
						PRINT_DEBUG("");
						if (transferTask.record(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, [&](VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue) {
									PRINT_DEBUG("");
									const VkImageMemoryBarrier vk_imageBarrier0 = {
										.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
										.pNext = nullptr,
										.srcAccessMask = VK_ACCESS_NONE,
										.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
										.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
										.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
										.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
										.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
										.image = vk_hWindowButtonImage,
										.subresourceRange = {
											.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
											.baseMipLevel = 0,
											.levelCount = 1,
											.baseArrayLayer = 0,
											.layerCount = WINDOW_WAYLAND_BUTTON_COUNT
										}
									};
									vkCmdPipelineBarrier(vk_hCommandBuffer, VK_PIPELINE_STAGE_NONE, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &vk_imageBarrier0);
									PRINT_DEBUG("");
									const VkBufferImageCopy vk_imageCopyRegions = {
										.bufferOffset = 0,
										.bufferRowLength = 0,
										.bufferImageHeight = 0,
										.imageSubresource = {
											.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
											.mipLevel = 0,
											.baseArrayLayer = 0,
											.layerCount = WINDOW_WAYLAND_BUTTON_COUNT
										},
										.imageOffset = {},
										.imageExtent = {
											.width = WINDOW_WAYLAND_BUTTON_TEXTURE_SIZE,
											.height = WINDOW_WAYLAND_BUTTON_TEXTURE_SIZE,
											.depth = 1
										}
									};
									vkCmdCopyBufferToImage(vk_hCommandBuffer, stagingWindowButtonBuffer.get(), vk_hWindowButtonImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vk_imageCopyRegions);
									PRINT_DEBUG("");
									const VkImageMemoryBarrier vk_imageBarrier = {
										.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
										.pNext = nullptr,
										.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
										.dstAccessMask = VK_ACCESS_NONE,
										.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
										.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
										.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
										.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
										.image = vk_hWindowButtonImage,
										.subresourceRange = {
											.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
											.baseMipLevel = 0,
											.levelCount = 1,
											.baseArrayLayer = 0,
											.layerCount = WINDOW_WAYLAND_BUTTON_COUNT
										}
									};
									vkCmdPipelineBarrier(vk_hCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &vk_imageBarrier);
									if (u8CurrentLogicalQueue == u8NextLogicalQueue)
										return;
									PRINT_DEBUG("");
									const VkImageMemoryBarrier vk_imageBarrier2 = {
										.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
										.pNext = nullptr,
										.srcAccessMask = VK_ACCESS_NONE,
										.dstAccessMask = VK_ACCESS_NONE,
										.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
										.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
										.srcQueueFamilyIndex = queueFamilyIndices[u8CurrentLogicalQueue],
										.dstQueueFamilyIndex = queueFamilyIndices[u8NextLogicalQueue],
										.image = vk_hWindowButtonImage,
										.subresourceRange = {
											.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
											.baseMipLevel = 0,
											.levelCount = 1,
											.baseArrayLayer = 0,
											.layerCount = WINDOW_WAYLAND_BUTTON_COUNT
										}
									};
									vkCmdPipelineBarrier(vk_hCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &vk_imageBarrier2);
								})) {
							PRINT_DEBUG("");
							if (transferTask.record(1, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, [&](VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue) {
										if (u8CurrentLogicalQueue == u8NextLogicalQueue)
											return;
										PRINT_DEBUG("");
										const VkImageMemoryBarrier vk_imageBarrier = {
											.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
											.pNext = nullptr,
											.srcAccessMask = VK_ACCESS_NONE,
											.dstAccessMask = VK_ACCESS_NONE,
											.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
											.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
											.srcQueueFamilyIndex = queueFamilyIndices[u8PreviousLogicalQueue],
											.dstQueueFamilyIndex = queueFamilyIndices[u8CurrentLogicalQueue],
											.image = vk_hWindowButtonImage,
											.subresourceRange = {
												.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
												.baseMipLevel = 0,
												.levelCount = 1,
												.baseArrayLayer = 0,
												.layerCount = WINDOW_WAYLAND_BUTTON_COUNT
											}
										};
										vkCmdPipelineBarrier(vk_hCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &vk_imageBarrier);
									})) {
								PRINT_DEBUG("");
								Vulkan_Fence transferFence(0);
								if (transferFence.valid()) {
									PRINT_DEBUG("");
									const VkPipelineStageFlags2 vk_aeInternStageWaits[] = {
										VK_PIPELINE_STAGE_2_TRANSFER_BIT | VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT
									};
									if (transferTask.submit(0, nullptr, vk_aeInternStageWaits, 0, nullptr, transferFence())) {
										PRINT_DEBUG("");
										transferFence.wait_for(std::numeric_limits<uint64_t>::max());
										return true;
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
		} else
			RE_FATAL_ERROR("Failed to create Vulkan image view for window button textures");
		return false;
	}

	void destroy_window_button_image() {
		PRINT_DEBUG("");
		vkDestroyImage(vk_hDevice, vk_hWindowButtonImage, nullptr);
	}

	void destroy_window_button_image_views() {
		PRINT_DEBUG("");
		vkDestroyImageView(vk_hDevice, vk_hWindowButtonImageView, nullptr);
	}

#else

	bool create_window_button_image() {
		return true;
	}

	bool create_window_button_image_views() {
		return true;
	}

	void destroy_window_button_image() {
	}

	void destroy_window_button_image_views() {
	}

#endif

}
