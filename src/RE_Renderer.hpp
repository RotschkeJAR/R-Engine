#ifndef __RE_RENDERER_H__
#define __RE_RENDERER_H__

#include "RE_Vulkan.hpp"

namespace RE {

#define RE_VK_RENDERABLE_RECTANGLES_COUNT 16384U
#define RE_VK_FRAMES_IN_FLIGHT 2U

	class SubRenderer {
		protected:
			Vulkan_CommandBuffer **ppSecondaryCommandBuffers;
			bool bValid;

		public:
			SubRenderer();
			~SubRenderer();
			virtual void record_secondary_command_buffer(const uint32_t u32CommandBufferIndex) const = 0;
			virtual void add_secondary_command_buffer(const Vulkan_CommandBuffer& rPrimaryCommandBuffer, const uint32_t u32FramebufferIndex) const = 0;
			virtual void render(const uint32_t u32CurrentFramebufferIndex, const uint8_t u8CurrentFrameInFlight) = 0;
			bool is_valid() const;
	};


	class Renderer_GameObject final : public SubRenderer {
		private:
			Vulkan_Shader gameObjectsVertexShader;
			Vulkan_Shader gameObjectsFragmentShader;
			Vulkan_PipelineLayout gameObjectsPipelineLayout;
			Vulkan_GraphicsPipeline gameObjectsGraphicsPipeline;
			Vulkan_Buffer gameObjectVertexBuffer, gameObjectVertexStagingBuffer;
			Vulkan_CommandBuffer vertexBufferTransferCommandBuffers[RE_VK_FRAMES_IN_FLIGHT];
			float *pVertices;
			uint16_t u16GameObjectsToRenderCount;

		public:
			const Vulkan_Semaphore semaphoreWaitForVertexBufferTransfer[RE_VK_FRAMES_IN_FLIGHT];

			Renderer_GameObject(const Vulkan_RenderPass *pRenderPass);
			~Renderer_GameObject();
			void record_secondary_command_buffer(const uint32_t u32CommandBufferIndex) const;
			void add_secondary_command_buffer(const Vulkan_CommandBuffer& rPrimaryCommandBuffer, const uint32_t u32FramebufferIndex) const;
			void render(const uint32_t u32CurrentFramebufferIndex, const uint8_t u8CurrentFrameInFlight);
	};

	extern Camera *pActiveCamera;
	
	class Renderer final {
		public:
			const Vulkan_RenderPass renderPass;

		private:
			Vulkan_CommandBuffer **ppPrimaryCommandBuffer;
			const Vulkan_Semaphore semaphoreAcquireSwapchainImage[RE_VK_FRAMES_IN_FLIGHT], semaphoreRenderFinished[RE_VK_FRAMES_IN_FLIGHT];
			const Vulkan_Fence renderFence[RE_VK_FRAMES_IN_FLIGHT], **ppSwapchainImageFences;
			uint8_t u8CurrentFrameInFlight;
			Renderer_GameObject gameObjectRenderer;
			bool bValid;

			void create_framebuffers();
			void destroy_framebuffers();
			void create_command_buffers();
			void destroy_command_buffers();
			void record_command_buffer(const uint32_t u32CommandBufferRecordIndex);

		public:
			const Vulkan_Buffer rectangleIndexBuffer;
			const Vulkan_Framebuffer **ppFramebuffers;
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
