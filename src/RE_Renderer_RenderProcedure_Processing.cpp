#include "RE_Renderer_RenderProcedure_Internal.hpp"

namespace RE {

	void process_game_objects(VkCommandBuffer vk_hCommandBuffer) {
		vkCmdBindPipeline(vk_hCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vk_hComputePipelineProcessing);
		const VkDescriptorSet vk_ahDescSets[] = {
			vk_ahSortableDepthDescSets[uCurrentFrameInFlightIndex],
			vk_ahGameObjectsDescSets[uCurrentFrameInFlightIndex],
			cameraDescSets[0]
		};
		vkCmdBindDescriptorSets(
				vk_hCommandBuffer,
				VK_PIPELINE_BIND_POINT_COMPUTE,
				vk_hProcessingPipelineLayout,
				0,
				static_cast<uint32_t>(sizeof(vk_ahDescSets) / sizeof(vk_ahDescSets[0])),
				vk_ahDescSets,
				0,
				nullptr);
		vkCmdDispatch(vk_hCommandBuffer, 1, 1, 1);
	}

}
