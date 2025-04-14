#ifndef __RE_RENDERER_H__
#define __RE_RENDERER_H__

#include "RE_Vulkan.hpp"
#include "RE_Vulkan_Command Buffer.hpp"

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

	class SubRenderer;
	
	class Renderer final {
		private:
			Vulkan_CommandBuffer primaryCommandBuffer;
			std::vector<SubRenderer*> subRenderers;
			bool bValid;

		public:
			Renderer();
			~Renderer();
			void render();
			void window_resize_event();
			bool is_valid() const;
	};

	class SubRenderer {
		protected:
			Vulkan_CommandBuffer secondaryCommandBuffer;
			bool bValid;

		public:
			SubRenderer();
			~SubRenderer();
			virtual void render() = 0;
			virtual void window_resize_event() = 0;
			bool is_valid() const;
	};

}

#endif /* __RE_RENDERER_H__ */
