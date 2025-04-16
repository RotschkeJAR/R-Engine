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

	class SubRenderer {
		protected:
			Vulkan_CommandBuffer secondaryCommandBuffer;
			bool bValid;

		public:
			SubRenderer();
			~SubRenderer();
			virtual void record_command_buffer(const Vulkan_CommandBuffer& commandBuffer) = 0;
			virtual void record_secondary_command_buffer() = 0;
			virtual void render() = 0;
			virtual void window_resize_event() = 0;
			const Vulkan_CommandBuffer* get_vulkan_command_buffer_ptr() const;
			VkCommandBuffer get_actual_command_buffer() const;
			bool is_valid() const;

			operator const Vulkan_CommandBuffer*() const;
			operator VkCommandBuffer() const;
	};


	class Renderer_GameObject final : public SubRenderer {
		public:
			Renderer_GameObject();
			~Renderer_GameObject();
			void record_command_buffer(const Vulkan_CommandBuffer& commandBuffer);
			void record_secondary_command_buffer();
			void render();
			void window_resize_event();
	};
	
	class Renderer final {
		private:
			Vulkan_CommandBuffer primaryCommandBuffer;
			Renderer_GameObject gameObjectRenderer;
			bool bValid;

		public:
			const Vulkan_Buffer rectangleIndexBuffer;

			Renderer();
			~Renderer();
			void render();
			void window_resize_event();
			bool is_valid() const;
	};

}

#endif /* __RE_RENDERER_H__ */
