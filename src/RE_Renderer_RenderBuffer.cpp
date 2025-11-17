#include "RE_Renderer_Internal.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_ListBatch_GameObject.hpp"
#include "RE_List_GameObject.hpp"

namespace RE {
	
	VkBuffer vk_hStagingRenderBuffer;
	VkDeviceMemory vk_hStagingRenderBufferMemory;
	VkDrawIndexedIndirectCommand *vk_pRenderBufferDrawCommand;
	GameObjectInstanceData *paRenderBufferInstanceData;

	VkBuffer vk_hRenderBuffer;
	VkDeviceMemory vk_hRenderBufferMemory;

	size_t gameObjectCountRenderBuffer;

	bool create_render_buffers() {
		PRINT_DEBUG("Creating staging render buffer in Vulkan");
		gameObjectCountRenderBuffer = std::clamp<size_t>(gameObjectBatchList.size(), 5, std::numeric_limits<size_t>::max()) * MAXIMUM_GAME_OBJECTS_PER_BATCH;
		const VkDeviceSize vk_renderBufferByteSize = gameObjectCountRenderBuffer * sizeof(GameObjectInstanceData) + sizeof(VkDrawIndexedIndirectCommand);
		if (create_vulkan_buffer(vk_renderBufferByteSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 1, nullptr, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &vk_hStagingRenderBuffer, &vk_hStagingRenderBufferMemory)) {
			PRINT_DEBUG("Mapping to staging render buffer's memory");
			void *pStagingRenderBufferContent;
			vkMapMemory(vk_hDevice, vk_hStagingRenderBufferMemory, 0, vk_renderBufferByteSize, 0, &pStagingRenderBufferContent);
			vk_pRenderBufferDrawCommand = reinterpret_cast<VkDrawIndexedIndirectCommand*>(pStagingRenderBufferContent);
			paRenderBufferInstanceData = reinterpret_cast<GameObjectInstanceData*>(vk_pRenderBufferDrawCommand + 1);
			PRINT_DEBUG("Creating render buffer on GPU");
			const uint32_t a2u32QueuesForRenderBuffer[2] = {queueFamilyIndices[get_render_buffer_transfer_queue_logical_index()], queueFamilyIndices[get_render_graphics_queue_logical_index()]};
			if (create_vulkan_buffer(vk_renderBufferByteSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, a2u32QueuesForRenderBuffer[0] == a2u32QueuesForRenderBuffer[1] ? 1 : 2, a2u32QueuesForRenderBuffer, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_hRenderBuffer, &vk_hRenderBufferMemory))
				return true;
			else
				RE_FATAL_ERROR("Failed to create render buffer on GPU");
			PRINT_DEBUG("Destroying staging render buffer due to failure creating render buffer on GPU");
			vkDestroyBuffer(vk_hDevice, vk_hStagingRenderBuffer, nullptr);
			vkFreeMemory(vk_hDevice, vk_hStagingRenderBufferMemory, nullptr);
		} else
			RE_FATAL_ERROR("Failed to create staging render buffer in Vulkan");
		return false;
	}

	void destroy_render_buffers() {
		PRINT_DEBUG("Destroying both staging and render buffer in Vulkan");
		vkDestroyBuffer(vk_hDevice, vk_hRenderBuffer, nullptr);
		vkFreeMemory(vk_hDevice, vk_hRenderBufferMemory, nullptr);
		vkUnmapMemory(vk_hDevice, vk_hStagingRenderBufferMemory);
		vkDestroyBuffer(vk_hDevice, vk_hStagingRenderBuffer, nullptr);
		vkFreeMemory(vk_hDevice, vk_hStagingRenderBufferMemory, nullptr);
	}

}
