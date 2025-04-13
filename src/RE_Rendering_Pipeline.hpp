#ifndef __RE_RENDERING_PIPELINE_H__
#define __RE_RENDERING_PIPELINE_H__

#include "RE_Vulkan.hpp"
#include "RE_Rendering_Shader.hpp"
#include "RE_Rendering_Pipeline Layout.hpp"
#include "RE_Rendering_Render Pass.hpp"

namespace RE {

	class Rendering_GraphicsPipeline final {
		private:
			VkPipeline vk_hGraphicsPipeline;

		public:
			Rendering_GraphicsPipeline() = delete;
			Rendering_GraphicsPipeline(const Rendering_Shader *pVertexShader, const Rendering_Shader *pFragmentShader, const uint32_t u32VertexInputBindingDescritpionCount, const VkVertexInputBindingDescription *vk_pVertexInputBindingDescritpions, const uint32_t u32VertexInputAttributeDescritpionCount, const VkVertexInputAttributeDescription *vk_pVertexInputAttributeDescritpions, const Rendering_PipelineLayout *pPipelineLayout, const Rendering_RenderPass *pRenderPass);
			~Rendering_GraphicsPipeline();
			VkPipeline get_graphics_pipeline() const;
			bool is_valid() const;

			operator VkPipeline() const;
	};

}

#endif /* __RE_RENDERING_PIPELINE_H__ */