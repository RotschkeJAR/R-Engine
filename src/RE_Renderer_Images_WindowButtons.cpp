#include "RE_Renderer_Images_Internal.hpp"
#include "RE_Window.hpp"
#include "RE_Asset.hpp"

namespace RE {
	
#ifdef RE_OS_LINUX

	VkImage vk_hWindowButtonImage;
	VkImageView vk_hWindowButtonImageView;

	bool create_window_button_image() {
		PRINT_DEBUG("Creating Vulkan image for window button textures");
		if (create_vulkan_image(
				0,
				VK_IMAGE_TYPE_2D,
				VK_FORMAT_R8_UNORM,
				VkExtent3D {
					.width = WINDOW_BUTTON_TEXTURE_SIZE,
					.height = WINDOW_BUTTON_TEXTURE_SIZE,
					.depth = 1
				},
				1,
				WINDOW_BUTTON_TEXTURE_COUNT,
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
		PRINT_DEBUG("Allocating staging Vulkan buffer for window button textures");
		constexpr VkDeviceSize vk_stagingBufferSize = (WINDOW_BUTTON_TEXTURE_SIZE * WINDOW_BUTTON_TEXTURE_SIZE) * WINDOW_BUTTON_TEXTURE_COUNT * sizeof(uint8_t);
		Vulkan_Buffer stagingWindowButtonBuffer(
				0,
				vk_stagingBufferSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				1,
				nullptr,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		if (stagingWindowButtonBuffer.valid()) {
			PRINT_DEBUG("Mapping staging Vulkan buffer for window button textures to RAM");
			void *pStagingBufferMemory;
			if (stagingWindowButtonBuffer.get_memory().map(0, 0, VK_WHOLE_SIZE, &pStagingBufferMemory)) {
				PRINT_DEBUG("Opening asset file containing window button textures");
				AssetImage windowButtonImage;
				if (asset_open("rsc/Linux/wayland_window_buttons.png", windowButtonImage.file)) {
					PRINT_DEBUG("Loading content of asset file containing window button textures");
					if (asset_image_load(windowButtonImage, 1)) {
						PRINT_DEBUG("Closing asset file containing window button textures");
						asset_close(windowButtonImage.file);
						PRINT_DEBUG("Copying content of asset file to staging Vulkan buffer");
						std::memcpy(pStagingBufferMemory, windowButtonImage.pBuffer, vk_stagingBufferSize);
						asset_image_free(windowButtonImage);
						stagingWindowButtonBuffer.get_memory().flush_mapped_memory(0, VK_WHOLE_SIZE);
						PRINT_DEBUG("Creating Vulkan task for transferring window button textures to GPU");
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
							PRINT_DEBUG("Recording 1st scope for Vulkan task transferring window button textures to GPU");
							if (transferTask.record(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, [&](VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue) {
										PRINT_DEBUG("Recording pipeline barrier for assigning an image layout for getting window button textures");
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
												.layerCount = WINDOW_BUTTON_TEXTURE_COUNT
											}
										};
										vkCmdPipelineBarrier(vk_hCommandBuffer, VK_PIPELINE_STAGE_NONE, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &vk_imageBarrier0);
										PRINT_DEBUG("Copying window button textures to image on GPU");
										const VkBufferImageCopy vk_imageCopyRegions = {
											.bufferOffset = 0,
											.bufferRowLength = 0,
											.bufferImageHeight = 0,
											.imageSubresource = {
												.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
												.mipLevel = 0,
												.baseArrayLayer = 0,
												.layerCount = WINDOW_BUTTON_TEXTURE_COUNT
											},
											.imageOffset = {},
											.imageExtent = {
												.width = WINDOW_BUTTON_TEXTURE_SIZE,
												.height = WINDOW_BUTTON_TEXTURE_SIZE,
												.depth = 1
											}
										};
										vkCmdCopyBufferToImage(vk_hCommandBuffer, stagingWindowButtonBuffer.get(), vk_hWindowButtonImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vk_imageCopyRegions);
										PRINT_DEBUG("Transferring image layout of image containing window button textures to read-only on GPU");
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
												.layerCount = WINDOW_BUTTON_TEXTURE_COUNT
											}
										};
										vkCmdPipelineBarrier(vk_hCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &vk_imageBarrier);
										if (u8CurrentLogicalQueue == u8NextLogicalQueue)
											return;
										PRINT_DEBUG("Releasing ownership of image containing window button textures for another queue on the GPU");
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
												.layerCount = WINDOW_BUTTON_TEXTURE_COUNT
											}
										};
										vkCmdPipelineBarrier(vk_hCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &vk_imageBarrier2);
									})) {
								PRINT_DEBUG("Recording 2nd scope for Vulkan task transferring window button textures to GPU");
								if (transferTask.record(1, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, [&](VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue) {
											if (u8CurrentLogicalQueue == u8NextLogicalQueue)
												return;
											PRINT_DEBUG("Acquiring ownership of image containing window button textures for another queue on the GPU");
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
													.layerCount = WINDOW_BUTTON_TEXTURE_COUNT
												}
											};
											vkCmdPipelineBarrier(vk_hCommandBuffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &vk_imageBarrier);
										})) {
									PRINT_DEBUG("Creating Vulkan fence for waiting for finishing transferring window button textures to GPU");
									Vulkan_Fence transferFence(0);
									if (transferFence.valid()) {
										PRINT_DEBUG("Submitting Vulkan task transferring window button textures to GPU");
										const VkPipelineStageFlags2 vk_aeInternStageWaits[] = {
											VK_PIPELINE_STAGE_2_TRANSFER_BIT | VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT
										};
										if (transferTask.submit(0, nullptr, vk_aeInternStageWaits, 0, nullptr, transferFence())) {
											PRINT_DEBUG("Creating Vulkan image view for window button textures");
											if (create_vulkan_image_view(
													0,
													vk_hWindowButtonImage,
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
														.layerCount = WINDOW_BUTTON_TEXTURE_COUNT
													},
													&vk_hWindowButtonImageView)) {
												PRINT_DEBUG("Waiting for finishing transferring window button textures to GPU");
												transferFence.wait_for(std::numeric_limits<uint64_t>::max());
												return true;
											} else
												RE_FATAL_ERROR("Failed to create Vulkan image view for window button textures");
											PRINT_DEBUG("");
											transferFence.wait_for(std::numeric_limits<uint64_t>::max());
										} else
											RE_FATAL_ERROR("Failed submitting Vulkan task to transfer window button textures to GPU");
									} else
										RE_FATAL_ERROR("Failed creating Vulkan fence for synchronizing with the transfer of window button textures to GPU");
								} else
									RE_FATAL_ERROR("Failed recording 2nd scope of the Vulkan task transferring window button textures to GPU");
							} else
								RE_FATAL_ERROR("Failed recording 1st scope of the Vulkan task transferring window button textures to GPU");
						} else
							RE_FATAL_ERROR("Failed creating Vulkan task to transfer window button textures to GPU");
					} else {
						RE_FATAL_ERROR("Failed loading content of asset file containing window button textures");
						PRINT_DEBUG("Closing asset file containing window button textures due to failure loading its content");
						asset_close(windowButtonImage.file);
					}
				} else
					RE_FATAL_ERROR("Failed opening asset file containing window button textures");
			} else
				RE_FATAL_ERROR("Failed mapping memory of staging Vulkan buffer to RAM");
		} else
			RE_FATAL_ERROR("Failed creating staging Vulkan buffer for transferring window button textures to GPU");
		return false;
	}

	void destroy_window_button_image() {
		PRINT_DEBUG("Destroying Vulkan image containing window button textures");
		vkDestroyImage(vk_hDevice, vk_hWindowButtonImage, nullptr);
	}

	void destroy_window_button_image_views() {
		PRINT_DEBUG("Destroying Vulkan image view pointing to window button textures");
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
