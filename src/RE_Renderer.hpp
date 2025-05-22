#ifndef __RE_RENDERER_H__
#define __RE_RENDERER_H__

#include "RE_Vulkan.hpp"

namespace RE {

#define RE_VK_RENDERABLE_RECTANGLES_COUNT 16384U
#define RE_VK_FRAMES_IN_FLIGHT 2U

	typedef float REgameObjectVertex_t;

	class SubRenderer {
		protected:
			bool is_subrenderer_valid() const;

		public:
			SubRenderer();
			~SubRenderer();
			virtual void record_secondary_command_buffer() const = 0;
			virtual void add_secondary_command_buffer() const = 0;
			virtual void render() = 0;
			virtual bool is_valid() const = 0;
	};


	class Renderer_GameObject final : public SubRenderer {
		private:
			uint16_t u16GameObjectsToRenderCount;

		public:
			Renderer_GameObject();
			~Renderer_GameObject();
			void record_secondary_command_buffer() const;
			void add_secondary_command_buffer() const;
			void render();
			bool is_valid() const;
	};

	extern Camera *pActiveCamera;
	
	class Renderer final {
		private:
			uint8_t u8CurrentFrameInFlight;
			Renderer_GameObject gameObjectRenderer;
			bool bValid;

			void create_framebuffers();
			void destroy_framebuffers();
			void create_command_buffers();
			void destroy_command_buffers();
			void record_command_buffer();

		public:
			VkViewport vk_cameraViewportArea;
			VkRect2D vk_cameraScissorArea;
			static Renderer *pInstance;

			Renderer();
			~Renderer();
			void render();
			void swapchain_recreated();
			void calculate_render_area();
			void wait_for_all_fences() const;
			bool is_valid() const;
	};

}

#endif /* __RE_RENDERER_H__ */
