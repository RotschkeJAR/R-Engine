#ifndef __RE_RENDERING_COMMAND_BUFFER_H__
#define __RE_RENDERING_COMMAND_BUFFER_H__

#include "RE_Vulkan.hpp"
#include "RE_Rendering_Render Pass.hpp"
#include "RE_Rendering_Framebuffer.hpp"
#include "RE_Rendering_Pipeline.hpp"
#include "RE_Rendering_Buffer.hpp"

namespace RE {
	
	class Rendering_CommandPool {
		private:
			VkCommandPool vk_hCommandPool;

		public:
			Rendering_CommandPool() = delete;
			Rendering_CommandPool(const VkCommandPoolCreateFlagBits vk_eCommandPoolCreateFlags, const uint32_t u32QueueFamilyIndex);
			~Rendering_CommandPool();
			VkCommandPool get_command_pool() const;
			bool is_valid() const;

			operator VkCommandPool() const;
	};

	class Rendering_CommandBuffer {
		private:
			VkCommandBuffer vk_hCommandBuffer;

		public:
			const VkCommandPool vk_hCommandPool;

			Rendering_CommandBuffer() = delete;
			Rendering_CommandBuffer(const Rendering_CommandPool *pCommandPool); // does not initialize the Vulkan command buffer
			Rendering_CommandBuffer(const VkCommandBufferLevel vk_eCommandBufferLevel, const Rendering_CommandPool *pCommandPool);
			~Rendering_CommandBuffer();

			void reset_command_buffer(const VkCommandBufferResetFlags vk_eCommandBufferResetFlags) const;
			bool begin_recording_command_buffer(const VkCommandBufferUsageFlags vk_eCommandBufferUsageFlags) const;
			bool end_recording_command_buffer() const;
			void cmd_begin_renderpass(const VkRenderPassBeginInfo vk_commandBufferRenderpassBeginInfo, const VkSubpassContents vk_eSubpassContents) const;
			void cmd_begin_renderpass(const float fClearColor[4], const int32_t i32ClearColor[4], const uint32_t u32ClearColor[4], const float fClearDepth, const uint32_t u32ClearStencil, const Rendering_RenderPass *pRenderPass, const Rendering_Framebuffer *pFramebuffer, const VkRect2D vk_renderArea, const VkSubpassContents vk_eSubpassContents) const;
			void cmd_end_renderpass() const;
			void cmd_bind_graphics_pipeline(const Rendering_GraphicsPipeline *pGraphicsPipeline) const;
			void cmd_bind_index_buffer(const Rendering_Buffer *pIndexBuffer, const VkIndexType vk_eIndexDatatype) const;
			void cmd_bind_vertex_buffer(const Rendering_Buffer *pVertexBuffer) const;
			void cmd_copy_buffer(const Rendering_Buffer *pSrcBuffer, const Rendering_Buffer *pDstBuffer, const VkDeviceSize vk_bufferSize) const;
			void cmd_set_viewport(const VkViewport vk_viewport) const;
			void cmd_set_viewport(const float fX, const float fY, const float fWidth, const float fHeight, const float fMinDepth, const float fMaxDepth) const;
			void cmd_set_scissor(const VkRect2D vk_scissorRect) const;
			void cmd_set_scissor(const VkOffset2D vk_scissorOffset, const VkExtent2D vk_scissorExtent) const;
			void cmd_set_scissor(const int32_t i32X, const int32_t i32Y, const uint32_t u32Width, const uint32_t u32Height) const;
			void cmd_draw(const uint32_t u32VertexCount, const uint32_t u32InstanceCount, const uint32_t u32FirstVertex, const uint32_t u32FirstInstance) const;
			void cmd_execute(const Rendering_CommandBuffer *pCommandBuffer) const;

			VkCommandBuffer get_command_buffer() const;
			bool is_valid() const;

			operator VkCommandBuffer() const;

		friend bool alloc_vk_command_buffers(const VkCommandBufferLevel vk_eCommandBufferLevel, const Rendering_CommandPool *pCommandPool, const uint32_t u32CommandBufferCount, Rendering_CommandBuffer **ppCommandBuffers);
	};

	bool alloc_vk_command_buffers(const VkCommandBufferLevel vk_eCommandBufferLevel, const Rendering_CommandPool *pCommandPool, const uint32_t u32CommandBufferCount, Rendering_CommandBuffer **ppCommandBuffers);
	void free_vk_command_buffers(const Rendering_CommandPool *pCommandPool, const uint32_t u32CommandBufferCount, Rendering_CommandBuffer **ppCommandBuffers);

}

#endif /* __RE_RENDERING_COMMAND_BUFFER_H__ */
