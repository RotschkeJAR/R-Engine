#include "RE_Internal Header.hpp"
#include "RE_Vulkan_Instance.hpp"
#include "RE_Render System.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace RE {

#define RE_VK_IMAGE_DATA_INDEX 0
#define RE_VK_IMAGE_MEMORY_DATA_INDEX 1
	
	Texture::Texture(const char *pcPathToImage) : a2pData{nullptr, nullptr} {
		int32_t i32ImgWidth, i32ImgHeight, i32ImgChannels;
		CATCH_SIGNAL(stbi_set_flip_vertically_on_load(true));
		uint8_t *const pu8ImgData = CATCH_SIGNAL_AND_RETURN(stbi_load(pcPathToImage, &i32ImgWidth, &i32ImgHeight, &i32ImgChannels, 0), uint8_t*);
		if (!pu8ImgData) {
			RE_ERROR("Failed to load an image from a file");
			return;
		}
		VkFormat vk_eImageFormat;
		switch (i32ImgChannels) {
			case 1U:
				vk_eImageFormat = VK_FORMAT_R8_SRGB;
				break;
			case 2U:
				vk_eImageFormat = VK_FORMAT_R8G8_SRGB;
				break;
			case 3U:
				vk_eImageFormat = VK_FORMAT_R8G8B8_SRGB;
				break;
			case 4U:
				vk_eImageFormat = VK_FORMAT_R8G8B8A8_SRGB;
				break;
			default:
				CATCH_SIGNAL(stbi_image_free(pu8ImgData));
				RE_ERROR(append_to_string("Unkown channel type. Amount of channels: ", i32ImgChannels));
				return;
		}

		const VkDeviceSize vk_imgByteSize = i32ImgWidth * i32ImgHeight * i32ImgChannels;
		constexpr uint32_t u32StagingImageBufferQueueCount = 1U, au32StagingImageBufferQueues[u32StagingImageBufferQueueCount] = {RE_VK_QUEUE_TRANSFER_INDEX};
		VkBuffer vk_hStagingImageBuffer;
		VkDeviceMemory vk_hStagingImageBufferMemory;
		if (CATCH_SIGNAL_AND_RETURN(create_vulkan_buffer(vk_imgByteSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, u32StagingImageBufferQueueCount, au32StagingImageBufferQueues, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &vk_hStagingImageBuffer, &vk_hStagingImageBufferMemory), bool)) {
			uint8_t *pu8BufferMemory;
			vkMapMemory(vk_hDevice, vk_hStagingImageBufferMemory, 0U, vk_imgByteSize, 0, (void**) &pu8BufferMemory);
			std::memcpy((void*) pu8BufferMemory, (void*) pu8ImgData, vk_imgByteSize);
			vkUnmapMemory(vk_hDevice, vk_hStagingImageBufferMemory);

			constexpr uint32_t u32ImageQueueCount = 1U, au32ImageQueues[u32ImageQueueCount] = {RE_VK_QUEUE_GRAPHICS_INDEX};
			if (CATCH_SIGNAL_AND_RETURN(create_vulkan_image(0, VK_IMAGE_TYPE_2D, vk_eImageFormat, VkExtent3D(i32ImgWidth, i32ImgHeight, 1U), 1U, 1U, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, u32ImageQueueCount, au32ImageQueues, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, (VkImage*) &a2pData[RE_VK_IMAGE_DATA_INDEX], (VkDeviceMemory*) &a2pData[RE_VK_IMAGE_MEMORY_DATA_INDEX]), bool)) {
				const VkFenceCreateInfo vk_fenceCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
					.flags = 0
				};
				VkFence vk_hImageTransferFence;
				if (vkCreateFence(vk_hDevice, &vk_fenceCreateInfo, nullptr, &vk_hImageTransferFence) == VK_SUCCESS) {
					VkCommandBuffer vk_hImageTransferCommandBuffer;
					if (CATCH_SIGNAL_AND_RETURN(alloc_vulkan_command_buffers(vk_ahCommandPools[RE_VK_COMMAND_POOL_TRANSFER_TRANSIENT_INDEX], VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1U, &vk_hImageTransferCommandBuffer), bool)) {
						if (CATCH_SIGNAL_AND_RETURN(begin_recording_vulkan_command_buffer(vk_hImageTransferCommandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr), bool)) {
							const VkImageMemoryBarrier vk_imageLayoutToTransferDst = {
								.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
								.srcAccessMask = VK_ACCESS_NONE,
								.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
								.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
								.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
								.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
								.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
								.image = (VkImage) a2pData[RE_VK_IMAGE_DATA_INDEX],
								.subresourceRange = {
									.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
									.baseMipLevel = 0U,
									.levelCount = 1U,
									.baseArrayLayer = 0U,
									.layerCount = 1U
								}
							};
							vkCmdPipelineBarrier(vk_hImageTransferCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0U, nullptr, 0U, nullptr, 1U, &vk_imageLayoutToTransferDst);

							const VkBufferImageCopy vk_bufferToImageCopy = {
								.bufferOffset = 0U,
								.bufferRowLength = 0U,
								.bufferImageHeight = 0U,
								.imageSubresource = {
									.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
									.mipLevel = 0U,
									.baseArrayLayer = 0U,
									.layerCount = 1U
								},
								.imageOffset = {
									.x = 0,
									.y = 0,
									.z = 0
								},
								.imageExtent = {
									.width = static_cast<uint32_t>(i32ImgWidth),
									.height = static_cast<uint32_t>(i32ImgHeight),
									.depth = 1U
								}
							};
							vkCmdCopyBufferToImage(vk_hImageTransferCommandBuffer, vk_hStagingImageBuffer, (VkImage) a2pData[RE_VK_IMAGE_DATA_INDEX], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1U, &vk_bufferToImageCopy);

							const VkImageMemoryBarrier vk_imageLayoutToShaderRead = {
								.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
								.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
								.dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
								.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
								.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
								.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
								.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
								.image = (VkImage) a2pData[RE_VK_IMAGE_DATA_INDEX],
								.subresourceRange = {
									.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
									.baseMipLevel = 0U,
									.levelCount = 1U,
									.baseArrayLayer = 0U,
									.layerCount = 1U
								}
							};
							vkCmdPipelineBarrier(vk_hImageTransferCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0U, nullptr, 0U, nullptr, 1U, &vk_imageLayoutToShaderRead);

							if (vkEndCommandBuffer(vk_hImageTransferCommandBuffer) == VK_SUCCESS) {
								if (submit_to_vulkan_queue(vk_ahDeviceQueueFamilies[RE_VK_QUEUE_TRANSFER_INDEX], 0U, nullptr, nullptr, 1U, &vk_hImageTransferCommandBuffer, 0U, nullptr, vk_hImageTransferFence) == VK_SUCCESS) {
									vkWaitForFences(vk_hDevice, 1U, &vk_hImageTransferFence, VK_TRUE, std::numeric_limits<uint64_t>::max());
								} else
									RE_ERROR("Failed submitting Vulkan command buffer to transfer texture to the GPU");
							} else
								RE_ERROR("Failed finishing recording Vulkan command buffer for transferring texture to the GPU");
						} else
							RE_ERROR("Failed beginning recording Vulkan command buffer for transferring texture to the GPU");
						vkFreeCommandBuffers(vk_hDevice, vk_ahCommandPools[RE_VK_COMMAND_POOL_TRANSFER_TRANSIENT_INDEX], 1U, &vk_hImageTransferCommandBuffer);
					} else
						RE_ERROR("Failed allocating Vulkan command buffer for transferring texture to the GPU");
					vkDestroyFence(vk_hDevice, vk_hImageTransferFence, nullptr);
				} else
					RE_ERROR("Failed creating Vulkan fence for waiting for the GPU getting the loaded texture");
			} else
				RE_ERROR("Failed creating Vulkan image on the GPU for the loaded texture");
			vkFreeMemory(vk_hDevice, vk_hStagingImageBufferMemory, nullptr);
			vkDestroyBuffer(vk_hDevice, vk_hStagingImageBuffer, nullptr);
		}
		CATCH_SIGNAL(stbi_image_free(pu8ImgData));
		PRINT_LN(is_valid());
	}

	Texture::~Texture() {
		if (a2pData[RE_VK_IMAGE_MEMORY_DATA_INDEX] != nullptr)
			vkFreeMemory(vk_hDevice, (VkDeviceMemory) a2pData[RE_VK_IMAGE_MEMORY_DATA_INDEX], nullptr);
		if (a2pData[RE_VK_IMAGE_DATA_INDEX] != nullptr)
			vkDestroyImage(vk_hDevice, (VkImage) a2pData[RE_VK_IMAGE_DATA_INDEX], nullptr);
	}

	bool Texture::is_valid() const {
		return a2pData[RE_VK_IMAGE_DATA_INDEX] && a2pData[RE_VK_IMAGE_MEMORY_DATA_INDEX];
	}

}
