#ifndef __RE_RENDERER_H__
#define __RE_RENDERER_H__

#include "RE_Vulkan.hpp"

namespace RE {

#define RE_VK_RENDERABLE_RECTANGLES_COUNT 16384U

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
			const Vulkan_CommandBuffer* get_vulkan_command_buffer_ptr() const;
			VkCommandBuffer get_actual_command_buffer() const;
			bool is_valid() const;

			operator const Vulkan_CommandBuffer*() const;
			operator VkCommandBuffer() const;
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
			uint32_t u32GameObjectsToRender;

		public:
			const Vulkan_Semaphore semaphoreWaitForVertexBufferTransfer;

			Renderer_GameObject(const Vulkan_RenderPass *pRenderPass);
			~Renderer_GameObject();
			void record_command_buffer(const Vulkan_CommandBuffer& commandBuffer);
			void record_secondary_command_buffer();
			void render();
	};
	
	class Renderer final {
		public:
			const Vulkan_RenderPass renderPass;

		private:
			const Vulkan_CommandBuffer **ppPrimaryCommandBuffer;
			VkSemaphore vk_semaphoresToWaitForBeforeRendering[2];
			Renderer_GameObject gameObjectRenderer;
			bool bValid;

			void create_framebuffers();
			void destroy_framebuffers();
			void record_command_buffers();

		public:
			const Vulkan_Buffer rectangleIndexBuffer;
			const Vulkan_Semaphore semaphoreAcquireSwapchainImage, semaphoreRenderFinished;
			const Vulkan_Fence renderFence;
			const Vulkan_Framebuffer **ppFramebuffers;
			static Renderer *pInstance;

			Renderer();
			~Renderer();
			void render();
			void window_resize_event();
			bool is_valid() const;
	};

}

#endif /* __RE_RENDERER_H__ */
