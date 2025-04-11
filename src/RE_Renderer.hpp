#ifndef __RE_RENDERER_H__
#define __RE_RENDERER_H__

#include "RE_Vulkan.hpp"

namespace RE {

	typedef float REvertex;
#define RE_VK_RENDERABLE_OBJECTS_COUNT 1000U
#define RE_VK_VERTEX_COUNT (RE_VK_RENDERABLE_OBJECTS_COUNT * 4U)
#define RE_VK_VERTEX_POSITION_SIZE 3U
#define RE_VK_VERTEX_POSITION_SIZE_BYTES (RE_VK_VERTEX_POSITION_SIZE * sizeof(REvertex))
#define RE_VK_VERTEX_POSITION_OFFSET 0U
#define RE_VK_VERTEX_POSITION_OFFSET_BYTES (RE_VK_VERTEX_POSITION_OFFSET * sizeof(REvertex))
#define RE_VK_VERTEX_COLOR_SIZE 4U
#define RE_VK_VERTEX_COLOR_SIZE_BYTES (RE_VK_VERTEX_COLOR_SIZE * sizeof(REvertex))
#define RE_VK_VERTEX_COLOR_OFFSET RE_VK_VERTEX_POSITION_SIZE
#define RE_VK_VERTEX_COLOR_OFFSET_BYTES (RE_VK_VERTEX_COLOR_OFFSET * sizeof(REvertex))
#define RE_VK_VERTEX_TOTAL_SIZE 7U
#define RE_VK_VERTEX_TOTAL_SIZE_BYTES (RE_VK_VERTEX_TOTAL_SIZE * sizeof(REvertex))
	
	class Renderer {
		private:
			/* Rendering_Shader vertexShader, fragmentShader;
			Rendering_RenderPass renderPass;
			Rendering_PipelineLayout pipelineLayout;
			Rendering_GraphicsPipeline graphicsPipeline;
			Rendering_Buffer vertexBuffer, indexBuffer;
			Rendering_CommandBuffer cmdBuffer; */
			bool bValid;

		public:
			Renderer();
			~Renderer();
			void prepare_render();
			bool is_valid();
	};

}

#endif /* __RE_RENDERER_H__ */
