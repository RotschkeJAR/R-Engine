#include "RE_RenderBatch_GameObject.hpp"
#include "RE_Manager.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_ListBatch_GameObject.hpp"

namespace RE {
	
	RenderBatch_GameObject::RenderBatch_GameObject(ListBatch_GameObject &rGameObjectBatch) : rGameObjectBatch(rGameObjectBatch), vk_hStagingUniformBuffer(VK_NULL_HANDLE), vk_hStagingUniformBufferMemory(VK_NULL_HANDLE), pStagingUniformBufferData(nullptr) {
		PRINT_DEBUG_CLASS("Constructing game object render batch");
		init();
	}
	
	RenderBatch_GameObject::~RenderBatch_GameObject() {
		PRINT_DEBUG_CLASS("Destructing game object render batch");
		destroy();
	}
	
	bool RenderBatch_GameObject::init() {
		PRINT_DEBUG_CLASS("Creating staging Vulkan uniform buffer");
		constexpr VkDeviceSize vk_stagingBufferSize = sizeof(GameObjectUniformData) * MAXIMUM_GAME_OBJECTS_PER_BATCH;
		if (create_vulkan_buffer(vk_stagingBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 1, nullptr, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vk_hStagingUniformBuffer, &vk_hStagingUniformBufferMemory)) {
			PRINT_DEBUG_CLASS("Mapping to the uniform buffer's memory ", vk_hStagingUniformBufferMemory);
			if (vkMapMemory(vk_hDevice, vk_hStagingUniformBufferMemory, 0, vk_stagingBufferSize, 0, reinterpret_cast<void**>(&pStagingUniformBufferData)) == VK_SUCCESS) {
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
								pStagingUniformBufferData[u16GameObjectIndex].a16fModelMatrix[u8ModelMatrixElementIndex] = 1.0f;
								break;
							default:
								pStagingUniformBufferData[u16GameObjectIndex].a16fModelMatrix[u8ModelMatrixElementIndex] = 0.0f;
								break;
						}
					pStagingUniformBufferData[u16GameObjectIndex].u32TextureId = 0;
				}
				return true;
			} else
				RE_FATAL_ERROR("Failed to map memory ", vk_hStagingUniformBufferMemory, " of Vulkan buffer ", vk_hStagingUniformBuffer);
			PRINT_DEBUG_CLASS("Destroying staging Vulkan uniform buffer ", vk_hStagingUniformBuffer, " due to failure mapping its memory ", vk_hStagingUniformBufferMemory);
			vkDestroyBuffer(vk_hDevice, vk_hStagingUniformBuffer, nullptr);
			vkFreeMemory(vk_hDevice, vk_hStagingUniformBufferMemory, nullptr);
		} else
			RE_FATAL_ERROR("Failed to create staging Vulkan uniform buffer for game objects");
		return false;
	}
	
	void RenderBatch_GameObject::destroy() {
		PRINT_DEBUG_CLASS("Destroying staging Vulkan uniform buffer ", vk_hStagingUniformBuffer, " and freeing its memory ", vk_hStagingUniformBufferMemory);
		vkUnmapMemory(vk_hDevice, vk_hStagingUniformBufferMemory);
		vkDestroyBuffer(vk_hDevice, vk_hStagingUniformBuffer, nullptr);
		vkFreeMemory(vk_hDevice, vk_hStagingUniformBufferMemory, nullptr);
	}
	
	void RenderBatch_GameObject::fetch_render_data() {
		for (uint16_t u16GameObjectIndex = 0; u16GameObjectIndex < rGameObjectBatch.size(); u16GameObjectIndex++) {
			const GameObject *const pGameObject = rGameObjectBatch.at(u16GameObjectIndex);
			PRINT_DEBUG_CLASS("Calculating model matrix for game object ", pGameObject);
			for (uint8_t u8DimensionIndex = 0; u8DimensionIndex < 3; u8DimensionIndex++) {
				pStagingUniformBufferData[u16GameObjectIndex].a16fModelMatrix[u8DimensionIndex * 5] = pGameObject->transform.scale[u8DimensionIndex];
				pStagingUniformBufferData[u16GameObjectIndex].a16fModelMatrix[u8DimensionIndex * 4 + 3] = pGameObject->transform.position[u8DimensionIndex];
			}
		}
	}

	void RenderBatch_GameObject::render_opaque() {
		PRINT_DEBUG_CLASS("Rendering opaque game objects");
	}

	void RenderBatch_GameObject::render_transparent() {
		PRINT_DEBUG_CLASS("Rendering transparent game objects");
	}

}
