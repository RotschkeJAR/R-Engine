#include "RE_RenderBatch_GameObject.hpp"
#include "RE_Manager.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_ListBatch_GameObject.hpp"
#include "RE_Sprite.hpp"

namespace RE {
	
	RenderBatch_GameObject::RenderBatch_GameObject(ListBatch_GameObject &rGameObjectBatch) : rGameObjectBatch(rGameObjectBatch), vk_hStagingInstanceBuffer(VK_NULL_HANDLE), vk_hStagingInstanceBufferMemory(VK_NULL_HANDLE), pStagingInstanceBufferData(nullptr) {
		PRINT_DEBUG_CLASS("Constructing game object render batch");
		if (vk_hDevice)
			init();
	}
	
	RenderBatch_GameObject::~RenderBatch_GameObject() {
		PRINT_DEBUG_CLASS("Destructing game object render batch");
		if (vk_hDevice)
			destroy();
	}
	
	bool RenderBatch_GameObject::init() {
		PRINT_DEBUG_CLASS("Creating staging Vulkan uniform buffer");
		constexpr VkDeviceSize vk_stagingBufferSize = sizeof(GameObjectInstanceData) * MAXIMUM_GAME_OBJECTS_PER_BATCH;
		if (create_vulkan_buffer(vk_stagingBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 1, nullptr, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vk_hStagingInstanceBuffer, &vk_hStagingInstanceBufferMemory)) {
			PRINT_DEBUG_CLASS("Mapping to the uniform buffer's memory ", vk_hStagingInstanceBufferMemory);
			if (vkMapMemory(vk_hDevice, vk_hStagingInstanceBufferMemory, 0, vk_stagingBufferSize, 0, reinterpret_cast<void**>(&pStagingInstanceBufferData)) == VK_SUCCESS) {
				for (uint16_t u16GameObjectIndex = 0; u16GameObjectIndex < MAXIMUM_GAME_OBJECTS_PER_BATCH; u16GameObjectIndex++) {
					for (uint8_t u8ModelMatrixElementIndex = 0; u8ModelMatrixElementIndex < 16; u8ModelMatrixElementIndex++)
						switch (u8ModelMatrixElementIndex) {
							case 0:
							case 5:
							case 10:
							case 3:
							case 7:
							case 11:
								continue;
							case 15:
								pStagingInstanceBufferData[u16GameObjectIndex].a16fModelMatrix[u8ModelMatrixElementIndex] = 1.0f;
								break;
							default:
								pStagingInstanceBufferData[u16GameObjectIndex].a16fModelMatrix[u8ModelMatrixElementIndex] = 0.0f;
								break;
						}
					pStagingInstanceBufferData[u16GameObjectIndex].u32TextureUniformId = 0;
				}
				return true;
			} else
				RE_FATAL_ERROR("Failed to map memory ", vk_hStagingInstanceBufferMemory, " of Vulkan buffer ", vk_hStagingInstanceBuffer);
			PRINT_DEBUG_CLASS("Destroying staging Vulkan uniform buffer ", vk_hStagingInstanceBuffer, " due to failure mapping its memory ", vk_hStagingInstanceBufferMemory);
			vkDestroyBuffer(vk_hDevice, vk_hStagingInstanceBuffer, nullptr);
			vkFreeMemory(vk_hDevice, vk_hStagingInstanceBufferMemory, nullptr);
		} else
			RE_FATAL_ERROR("Failed to create staging Vulkan uniform buffer for game objects");
		return false;
	}
	
	void RenderBatch_GameObject::destroy() {
		PRINT_DEBUG_CLASS("Destroying staging Vulkan uniform buffer ", vk_hStagingInstanceBuffer, " and freeing its memory ", vk_hStagingInstanceBufferMemory);
		vkUnmapMemory(vk_hDevice, vk_hStagingInstanceBufferMemory);
		vkDestroyBuffer(vk_hDevice, vk_hStagingInstanceBuffer, nullptr);
		vkFreeMemory(vk_hDevice, vk_hStagingInstanceBufferMemory, nullptr);
	}
	
	void RenderBatch_GameObject::fetch_render_data() {
		for (uint16_t u16GameObjectIndex = 0; u16GameObjectIndex < rGameObjectBatch.size(); u16GameObjectIndex++) {
			const GameObject *const pGameObject = rGameObjectBatch.at(u16GameObjectIndex);
			PRINT_DEBUG_CLASS("Calculating model matrix for game object ", pGameObject);
			for (uint8_t u8DimensionIndex = 0; u8DimensionIndex < 3; u8DimensionIndex++) {
				pStagingInstanceBufferData[u16GameObjectIndex].a16fModelMatrix[u8DimensionIndex * 5] = pGameObject->transform.scale[u8DimensionIndex];
				pStagingInstanceBufferData[u16GameObjectIndex].a16fModelMatrix[u8DimensionIndex * 4 + 3] = pGameObject->transform.position[u8DimensionIndex];
			}
			pStagingInstanceBufferData[u16GameObjectIndex].u32TextureUniformId = reinterpret_cast<VulkanSprite*>(pGameObject->spriteRenderer.hSprite)->u16UniformIndex;
		}
	}

}
