#include "RE_Texture.hpp"
#include "RE_Vulkan_Device.hpp"
#include "RE_Render System.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_List_GameObject.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#define STBI_ASSERT(x)
#include "stb_image.h"

namespace RE {

	Texture alloc_texture_from_binary_data(const uint8_t *const pau8ImageBinaryData, const Vector2u imageSize, const uint32_t u32Channels) {
		return PUSH_TO_CALLSTACKTRACE_AND_RETURN(alloc_texture_from_binary_data(pau8ImageBinaryData, imageSize[0], imageSize[1], u32Channels), Texture);
	}

	Texture alloc_texture_from_binary_data(const uint8_t *const pau8ImageBinaryData, const uint32_t u32Width, const uint32_t u32Height, const uint32_t u32Channels) {
		/*if (u32Width > vk_physicalDeviceLimits.maxImageDimension2D || u32Height > vk_physicalDeviceLimits.maxImageDimension2D) {
			RE_ERROR("The binary image data cannot be used due to exceeding the GPU hardware limit of ", vk_physicalDeviceLimits.maxImageDimension2D, " pixels");
			return nullptr;
		}
		VkFormat vk_eImageFormat;
		switch (u32Channels) {
			case 1:
				vk_eImageFormat = VK_FORMAT_R8_SRGB;
				break;
			case 2:
				vk_eImageFormat = VK_FORMAT_R8G8_SRGB;
				break;
			case 3:
				vk_eImageFormat = VK_FORMAT_R8G8B8_SRGB;
				break;
			case 4:
				vk_eImageFormat = VK_FORMAT_R8G8B8A8_SRGB;
				break;
			default:
				RE_ERROR("The binary image data has ", u32Channels, " channels, which is unusual. The engine can load and use only 2D textures");
				return nullptr;
		}
		constexpr uint32_t u32StagingBufferQueueCount = 1;
		const std::array<uint32_t, u32StagingBufferQueueCount> u32StagingBufferQueues = {
			{
				RE_VK_QUEUE_TRANSFER_INDEX
			}
		};
		const VkDeviceSize vk_imgSizeInBytes = u32Width * u32Height * u32Channels;
		Vulkan_Buffer stagingTextureBuffer(vk_imgSizeInBytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, u32StagingBufferQueueCount, u32StagingBufferQueues.data(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		if (stagingTextureBuffer.is_valid()) {
			void *pStagingBufferData;
			stagingTextureBuffer.map_memory(&pStagingBufferData);
			std::memcpy(pStagingBufferData, pau8ImageBinaryData, vk_imgSizeInBytes);
			stagingTextureBuffer.unmap_memory();

			TextureContainer *const pTextureContainer = new TextureContainer();
			constexpr uint32_t u32QueueCount = 2;
			const std::array<uint32_t, u32QueueCount> au32Queues = {
				{
					RE_VK_QUEUE_GRAPHICS_INDEX,
					RE_VK_QUEUE_TRANSFER_INDEX
				}
			};
			if (create_vulkan_image(0, VK_IMAGE_TYPE_2D, vk_eImageFormat, VkExtent3D{u32Width, u32Height, 1}, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, u32QueueCount, au32Queues.data(), VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &pTextureContainer->vk_hImage, &pTextureContainer->vk_hImageMemory)) {
				if (create_vulkan_image_view(pTextureContainer->vk_hImage, VK_IMAGE_VIEW_TYPE_2D, vk_eImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1, &pTextureContainer->vk_hImgView)) {
					Vulkan_CommandBuffer textureTransferCommandBuffer(vk_ahCommandPools[RE_VK_COMMAND_POOL_TRANSFER_TRANSIENT_INDEX], VK_COMMAND_BUFFER_LEVEL_PRIMARY);
					if (textureTransferCommandBuffer.is_valid()) {
						if (begin_recording_vulkan_command_buffer(textureTransferCommandBuffer.get_command_buffer(), VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr)) {
							vk_cmd_transit_image(textureTransferCommandBuffer.get_command_buffer(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_ACCESS_NONE, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, pTextureContainer->vk_hImage, VK_IMAGE_ASPECT_COLOR_BIT);
							const VkBufferImageCopy vk_copyBufferToImage = {
								.bufferOffset = 0,
								.bufferRowLength = 0,
								.bufferImageHeight = 0,
								.imageSubresource = {
									.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
									.mipLevel = 0,
									.baseArrayLayer = 0,
									.layerCount = 1
								},
								.imageOffset = {
									.x = 0,
									.y = 0,
									.z = 0
								},
								.imageExtent = {
									.width = u32Width,
									.height = u32Height,
									.depth = 1
								}
							};
							vkCmdCopyBufferToImage(textureTransferCommandBuffer.get_command_buffer(), stagingTextureBuffer.get_buffer(), pTextureContainer->vk_hImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vk_copyBufferToImage);
							vk_cmd_transit_image(textureTransferCommandBuffer.get_command_buffer(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_ACCESS_NONE, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, pTextureContainer->vk_hImage, VK_IMAGE_ASPECT_COLOR_BIT);
							if (vkEndCommandBuffer(textureTransferCommandBuffer.get_command_buffer()) == VK_SUCCESS) {
								if (submit_to_vulkan_queue(vk_ahDeviceQueueFamilies[RE_VK_QUEUE_TRANSFER_INDEX], 0, nullptr, nullptr, 1, textureTransferCommandBuffer.get_command_buffer_ptr(), 0, nullptr, VK_NULL_HANDLE)) {
									vkQueueWaitIdle(vk_ahDeviceQueueFamilies[RE_VK_QUEUE_TRANSFER_INDEX]);
									return reinterpret_cast<Texture>(pTextureContainer);
								} else
									RE_ERROR("Failed to submit a Vulkan command buffer to the transer-queue to transfer the texture to the GPU");
							} else
								RE_ERROR("Failed to finish recording the Vulkan command buffer to transfer the texture to the GPU");
						} else
							RE_ERROR("Failed to start recording the Vulkan command buffer to transfer the texture to the GPU");
					} else
						RE_ERROR("Failed to allocate a temporary Vulkan command buffer to transfer the texture to the GPU");
					vkDestroyImageView(vk_hDevice, pTextureContainer->vk_hImgView, nullptr);
				} else
					RE_ERROR("Failed to create Vulkan image view on the GPU for rendering the texture");
				vkFreeMemory(vk_hDevice, pTextureContainer->vk_hImageMemory, nullptr);
				vkDestroyImage(vk_hDevice, pTextureContainer->vk_hImage, nullptr);
			} else
				RE_ERROR("Failed to create Vulkan image on the GPU for storing the texture in its memory");
			delete pTextureContainer;
		} else
			RE_ERROR("Failed to create a Vulkan staging buffer for transferring the recently loaded texture to the GPU");*/
		return nullptr;
	}
	
	Texture alloc_texture_loading_from_file(const char *const pacPathToTextureFile) {
		Vector2i textureSize;
		return PUSH_TO_CALLSTACKTRACE_AND_RETURN(alloc_texture_loading_from_file(pacPathToTextureFile, textureSize), Texture);
	}

	Texture alloc_texture_loading_from_file(const char *const pacPathToTextureFile, Vector2i &rSize) {
		return PUSH_TO_CALLSTACKTRACE_AND_RETURN(alloc_texture_loading_from_file(pacPathToTextureFile, rSize[0], rSize[1]), Texture);
	}
	
	Texture alloc_texture_loading_from_file(const char *const pacPathToTextureFile, int32_t &ri32Width, int32_t &ri32Height) {
		int32_t i32Channels;
		return PUSH_TO_CALLSTACKTRACE_AND_RETURN(alloc_texture_loading_from_file(pacPathToTextureFile, ri32Width, ri32Height, i32Channels), Texture);
	}
	
	Texture alloc_texture_loading_from_file(const char *const pacPathToTextureFile, Vector2i &rSize, int32_t &ri32Channels) {
		return PUSH_TO_CALLSTACKTRACE_AND_RETURN(alloc_texture_loading_from_file(pacPathToTextureFile, rSize[0], rSize[1], ri32Channels), Texture);
	}
	
	Texture alloc_texture_loading_from_file(const char *const pacPathToTextureFile, int32_t &ri32Width, int32_t &ri32Height, int32_t &ri32Channels) {
		/*if (vk_hDevice == VK_NULL_HANDLE) {
			RE_ERROR("Textures can't be loaded, when the engine doesn't run");
			return nullptr;
		}
		uint8_t *const pau8ImageBinaryData = PUSH_TO_CALLSTACKTRACE_AND_RETURN(stbi_load(pacPathToTextureFile, &ri32Width, &ri32Height, &ri32Channels, 0), uint8_t*);
		if (pau8ImageBinaryData) {
			const Texture hTexture = PUSH_TO_CALLSTACKTRACE_AND_RETURN(alloc_texture_from_binary_data(pau8ImageBinaryData, static_cast<uint32_t>(ri32Width), static_cast<uint32_t>(ri32Height), static_cast<uint32_t>(ri32Channels)), Texture);
			stbi_image_free(reinterpret_cast<void*>(pau8ImageBinaryData));
			return hTexture;
		}*/
		return nullptr;
	}

	void free_texture(const Texture hTexture) {
		if (!hTexture)
			return;
		else if (vk_hDevice == VK_NULL_HANDLE) {
			RE_ERROR("Textures can't be deleted from memory, when the engine doesn't run");
			return;
		}
		const TextureContainer *const pTextureContainer = reinterpret_cast<TextureContainer*>(hTexture);
		WAIT_FOR_IDLE_VULKAN_DEVICE();
		/*vkDestroyImageView(vk_hDevice, pTextureContainer->vk_hImgView, nullptr);
		vkFreeMemory(vk_hDevice, pTextureContainer->vk_hImageMemory, nullptr);
		vkDestroyImage(vk_hDevice, pTextureContainer->vk_hImage, nullptr);*/
		delete pTextureContainer;
	}

	void free_texture_and_fix_dangling_pointers(const Texture hTexture) {
		if (!hTexture)
			return;
		PUSH_TO_CALLSTACKTRACE(free_texture(hTexture));
		for (ListBatch_GameObject *const pGameObjectListBatch : gameObjectBatchList)
			for (uint16_t u16Index = 0; u16Index < pGameObjectListBatch->size(); u16Index++) {
				Texture &rTextureAttrib = pGameObjectListBatch->get(u16Index)->spriteRenderer.sprite.hTexture;
				if (rTextureAttrib == hTexture)
					rTextureAttrib = nullptr;
			}
	}

}
