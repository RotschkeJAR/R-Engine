#ifndef __RE_VULKAN_PIPELINE_H__
#define __RE_VULKAN_PIPELINE_H__

#include "RE_Vulkan.hpp"
#include "RE_Vulkan_Shader.hpp"
#include "RE_Vulkan_Pipeline Layout.hpp"
#include "RE_Vulkan_Render Pass.hpp"

namespace RE {

	class Vulkan_GraphicsPipeline final {
		private:
			VkPipeline vk_hGraphicsPipeline;

		public:
			Vulkan_GraphicsPipeline() = delete;
			Vulkan_GraphicsPipeline(const Vulkan_Shader *pVertexShader, const Vulkan_Shader *pFragmentShader, const uint32_t u32VertexInputBindingDescritpionCount, const VkVertexInputBindingDescription *vk_pVertexInputBindingDescritpions, const uint32_t u32VertexInputAttributeDescritpionCount, const VkVertexInputAttributeDescription *vk_pVertexInputAttributeDescritpions, const Vulkan_PipelineLayout *pPipelineLayout, const Vulkan_RenderPass *pRenderPass);
			~Vulkan_GraphicsPipeline();
			VkPipeline get_graphics_pipeline() const;
			bool is_valid() const;

			operator VkPipeline() const;
	};

}

#endif /* __RE_VULKAN_PIPELINE_H__ */