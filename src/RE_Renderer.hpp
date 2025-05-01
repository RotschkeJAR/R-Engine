#ifndef __RE_RENDERER_H__
#define __RE_RENDERER_H__

#include "RE_Vulkan.hpp"

namespace RE {

#define RE_VK_RENDERABLE_RECTANGLES_COUNT 16384U

	class SubRenderer {
		protected:
			Vulkan_CommandBuffer **ppSecondaryCommandBuffers;
			bool bValid;

		public:
			SubRenderer();
			~SubRenderer();
			virtual void record_secondary_command_buffer(const uint32_t u32CommandBufferIndex) const = 0;
			virtual void add_secondary_command_buffer(const Vulkan_CommandBuffer& rPrimaryCommandBuffer, const uint32_t u32FramebufferIndex) const = 0;
			virtual void render(const uint32_t u32CurrentFramebufferIndex) = 0;
			bool is_valid() const;
	};


	class Renderer_GameObject final : public SubRenderer {
		private:
			Vulkan_Shader gameObjectsVertexShader;
			Vulkan_Shader gameObjectsFragmentShader;
			Vulkan_PipelineLayout gameObjectsPipelineLayout;
			Vulkan_GraphicsPipeline gameObjectsGraphicsPipeline;
			Vulkan_Buffer gameObjectVertexBuffer, gameObjectVertexStagingBuffer;
			Vulkan_CommandBuffer vertexBufferTransferCommandBuffer;
			float *pVertices;
			uint16_t u16GameObjectsToRenderCount;

		public:
			const Vulkan_Semaphore semaphoreWaitForVertexBufferTransfer;

			Renderer_GameObject(const Vulkan_RenderPass *pRenderPass);
			~Renderer_GameObject();
			void record_secondary_command_buffer(const uint32_t u32CommandBufferIndex) const;
			void add_secondary_command_buffer(const Vulkan_CommandBuffer& rPrimaryCommandBuffer, const uint32_t u32FramebufferIndex) const;
			void render(const uint32_t u32CurrentFramebufferIndex);
	};

	extern Camera *pActiveCamera;
	
	class Renderer final {
		public:
			const Vulkan_RenderPass renderPass;

		private:
			Vulkan_CommandBuffer **ppPrimaryCommandBuffer;
			VkSemaphore vk_semaphoresToWaitForBeforeRendering[2];
			Renderer_GameObject gameObjectRenderer;
			bool bValid;

			void create_framebuffers();
			void destroy_framebuffers();
			void create_command_buffers();
			void destroy_command_buffers();
			void calculate_render_area();
			void record_command_buffer(const uint32_t u32CommandBufferRecordIndex);

		public:
			const Vulkan_Buffer rectangleIndexBuffer;
			const Vulkan_Semaphore semaphoreAcquireSwapchainImage, semaphoreRenderFinished;
			const Vulkan_Fence renderFence;
			const Vulkan_Framebuffer **ppFramebuffers;
			VkViewport vk_maxViewportArea;
			VkRect2D vk_maxScissorArea;
			static Renderer *pInstance;

			Renderer();
			~Renderer();
			void render();
			void window_resize_event();
			bool is_valid() const;
	};

}

#endif /* __RE_RENDERER_H__ */
