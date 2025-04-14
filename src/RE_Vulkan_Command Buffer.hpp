#ifndef __RE_VULKAN_COMMAND_BUFFER_H__
#define __RE_VULKAN_COMMAND_BUFFER_H__

#include "RE_Vulkan.hpp"
#include "RE_Vulkan_Render Pass.hpp"
#include "RE_Vulkan_Framebuffer.hpp"
#include "RE_Vulkan_Pipeline.hpp"
#include "RE_Vulkan_Buffer.hpp"

namespace RE {
	
	class Vulkan_CommandPool final {
		private:
			VkCommandPool vk_hCommandPool;

		public:
			Vulkan_CommandPool() = delete;
			Vulkan_CommandPool(const VkCommandPoolCreateFlagBits vk_eCommandPoolCreateFlags, const uint32_t u32QueueFamilyIndex);
			~Vulkan_CommandPool();
			VkCommandPool get_command_pool() const;
			bool is_valid() const;

			operator VkCommandPool() const;
	};

	class Vulkan_CommandBuffer {
		private:
			VkCommandBuffer vk_hCommandBuffer;

		public:
			const VkCommandPool vk_hCommandPool;

			Vulkan_CommandBuffer() = delete;
			Vulkan_CommandBuffer(const Vulkan_CommandPool *pCommandPool); // does not initialize the Vulkan command buffer
			Vulkan_CommandBuffer(const VkCommandBufferLevel vk_eCommandBufferLevel, const Vulkan_CommandPool *pCommandPool);
			~Vulkan_CommandBuffer();

			void reset_command_buffer(const VkCommandBufferResetFlags vk_eCommandBufferResetFlags) const;
			bool begin_recording_command_buffer(const VkCommandBufferUsageFlags vk_eCommandBufferUsageFlags) const;
			bool end_recording_command_buffer() const;
			void cmd_begin_renderpass(const VkRenderPassBeginInfo vk_commandBufferRenderpassBeginInfo, const VkSubpassContents vk_eSubpassContents) const;
			void cmd_begin_renderpass(const float fClearColor[4], const int32_t i32ClearColor[4], const uint32_t u32ClearColor[4], const float fClearDepth, const uint32_t u32ClearStencil, const Vulkan_RenderPass *pRenderPass, const Vulkan_Framebuffer *pFramebuffer, const VkRect2D vk_renderArea, const VkSubpassContents vk_eSubpassContents) const;
			void cmd_end_renderpass() const;
			void cmd_bind_pipeline(const VkPipelineBindPoint vk_ePipelineBindPoint, const VkPipeline vk_pipeline) const;
			void cmd_bind_graphics_pipeline(const Vulkan_GraphicsPipeline *pGraphicsPipeline) const;
			void cmd_bind_index_buffer(const VkBuffer vk_indexBuffer, const VkIndexType vk_eIndexDatatype) const;
			void cmd_bind_index_buffer(const Vulkan_Buffer *pIndexBuffer, const VkIndexType vk_eIndexDatatype) const;
			void cmd_bind_vertex_buffer(const VkBuffer *vk_pVertexBuffers, const VkDeviceSize *vk_pOffsets) const;
			void cmd_bind_vertex_buffer(const Vulkan_Buffer *pVertexBuffer, const VkDeviceSize vk_offset) const;
			void cmd_copy_buffer(const VkBuffer vk_srcBuffer, const VkDeviceSize vk_srcOffset, const VkBuffer dstBuffer, const VkDeviceSize vk_dstOffset, const VkDeviceSize vk_bufferSize) const;
			void cmd_copy_buffer(const VkBuffer vk_srcBuffer, const VkBuffer dstBuffer, const VkDeviceSize vk_bufferSize) const;
			void cmd_copy_buffer(const Vulkan_Buffer *pSrcBuffer, const VkDeviceSize vk_srcOffset, const Vulkan_Buffer *pDstBuffer, const VkDeviceSize vk_dstOffset, const VkDeviceSize vk_bufferSize) const;
			void cmd_copy_buffer(const Vulkan_Buffer *pSrcBuffer, const Vulkan_Buffer *pDstBuffer, const VkDeviceSize vk_bufferSize) const;
			void cmd_set_viewport(const VkViewport vk_viewport) const;
			void cmd_set_viewport(const float fX, const float fY, const float fWidth, const float fHeight, const float fMinDepth, const float fMaxDepth) const;
			void cmd_set_scissor(const VkRect2D vk_scissorRect) const;
			void cmd_set_scissor(const VkOffset2D vk_scissorOffset, const VkExtent2D vk_scissorExtent) const;
			void cmd_set_scissor(const int32_t i32X, const int32_t i32Y, const uint32_t u32Width, const uint32_t u32Height) const;
			void cmd_draw(const uint32_t u32VertexCount, const uint32_t u32InstanceCount, const uint32_t u32FirstVertex, const uint32_t u32FirstInstance) const;
			void cmd_execute(const Vulkan_CommandBuffer *pCommandBuffer) const;

			VkCommandBuffer get_command_buffer() const;
			bool is_valid() const;

			operator VkCommandBuffer() const;

		friend bool alloc_vk_command_buffers(const VkCommandBufferLevel vk_eCommandBufferLevel, const Vulkan_CommandPool *pCommandPool, const uint32_t u32CommandBufferCount, Vulkan_CommandBuffer **ppCommandBuffers);
	};

	bool alloc_vk_command_buffers(const VkCommandBufferLevel vk_eCommandBufferLevel, const Vulkan_CommandPool *pCommandPool, const uint32_t u32CommandBufferCount, Vulkan_CommandBuffer **ppCommandBuffers);
	void free_vk_command_buffers(const Vulkan_CommandPool *pCommandPool, const uint32_t u32CommandBufferCount, Vulkan_CommandBuffer **ppCommandBuffers);

}

#endif /* __RE_VULKAN_COMMAND_BUFFER_H__ */
