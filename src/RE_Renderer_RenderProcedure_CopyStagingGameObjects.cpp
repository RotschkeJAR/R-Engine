#include "RE_Renderer_RenderProcedure_Internal.hpp"

namespace RE {

	void copy_staging_game_objects(VkCommandBuffer vk_hCommandBuffer) {
		memset(paStagingGameObjectsBufferData, 0, get_max_game_object_count() * sizeof(GameObjectShaderData));
		paStagingGameObjectsBufferData[0].a3fPosition[0] = 0.0f;
		paStagingGameObjectsBufferData[0].a3fPosition[1] = 0.0f;
		paStagingGameObjectsBufferData[0].a3fPosition[2] = 0.0f;
		paStagingGameObjectsBufferData[0].a3fRotation[0] = 0.0f;
		paStagingGameObjectsBufferData[0].a3fRotation[1] = 0.0f;
		paStagingGameObjectsBufferData[0].a3fRotation[2] = 0.0f;
		paStagingGameObjectsBufferData[0].a3fScale[0] = 1.0f;
		paStagingGameObjectsBufferData[0].a3fScale[1] = 1.0f;
		paStagingGameObjectsBufferData[0].a3fScale[2] = 1.0f;
		paStagingGameObjectsBufferData[0].a4fColor[0] = 1.0f;
		paStagingGameObjectsBufferData[0].a4fColor[1] = 0.0f;
		paStagingGameObjectsBufferData[0].a4fColor[2] = 0.0f;
		paStagingGameObjectsBufferData[0].a4fColor[3] = 1.0f;
		paStagingGameObjectsBufferData[0].u32TextureId = DONT_USE_TEXTURE;
		const VkBufferCopy vk_bufferCopyInfo = {
			.srcOffset = 0,
			.dstOffset = 0,
			.size = get_max_game_object_count() * sizeof(GameObjectShaderData)
		};
		vkCmdCopyBuffer(vk_hCommandBuffer, vk_hStagingGameObjectsBuffer, vk_ahGameObjectsBuffers[uCurrentFrameInFlightIndex], 1, &vk_bufferCopyInfo);
	}

}
