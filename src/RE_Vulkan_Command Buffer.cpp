#include "RE_Vulkan.hpp"
#include "RE_Render System.hpp"

namespace RE {
	
	Vulkan_CommandPool::Vulkan_CommandPool(const VkCommandPoolCreateFlagBits vk_eCommandPoolCreateFlags, const uint32_t u32QueueFamilyIndex) : vk_hCommandPool(VK_NULL_HANDLE) {
		VkCommandPoolCreateInfo vk_commandPoolCreateInfo = {};
		vk_commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		vk_commandPoolCreateInfo.flags = vk_eCommandPoolCreateFlags;
		vk_commandPoolCreateInfo.queueFamilyIndex = u32QueueFamilyIndex;
		if (!CHECK_VK_RESULT(vkCreateCommandPool(vk_hDevice, &vk_commandPoolCreateInfo, nullptr, &vk_hCommandPool))) {
			RE_ERROR("Failed to create a Vulkan command pool");
			vk_hCommandPool = VK_NULL_HANDLE;
		}
	}

	Vulkan_CommandPool::~Vulkan_CommandPool() {
		if (!is_valid())
			return;
		CATCH_SIGNAL(vkDestroyCommandPool(vk_hDevice, vk_hCommandPool, nullptr));
	}

	VkCommandPool Vulkan_CommandPool::get_command_pool() const {
		return vk_hCommandPool;
	}

	bool  Vulkan_CommandPool::is_valid() const {
		return vk_hCommandPool != VK_NULL_HANDLE;
	}

	Vulkan_CommandPool::operator VkCommandPool() const {
		return this->vk_hCommandPool;
	}


	
	Vulkan_CommandBuffer::Vulkan_CommandBuffer(const Vulkan_CommandPool *pCommandPool) : vk_hCommandBuffer(VK_NULL_HANDLE), vk_hCommandPool(*pCommandPool) {}
	Vulkan_CommandBuffer::Vulkan_CommandBuffer(const VkCommandBufferLevel vk_eCommandBufferLevel, const Vulkan_CommandPool *pCommandPool) : vk_hCommandBuffer(VK_NULL_HANDLE), vk_hCommandPool(*pCommandPool) {
		VkCommandBufferAllocateInfo vk_commandBufferAllocInfo = {};
		vk_commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		vk_commandBufferAllocInfo.level = vk_eCommandBufferLevel;
		vk_commandBufferAllocInfo.commandPool = *pCommandPool;
		vk_commandBufferAllocInfo.commandBufferCount = 1U;
		if (!CHECK_VK_RESULT(vkAllocateCommandBuffers(vk_hDevice, &vk_commandBufferAllocInfo, &vk_hCommandBuffer))) {
			RE_ERROR("Failed allocating Vulkan command buffer");
			vk_hCommandBuffer = VK_NULL_HANDLE;
		}
	}

	Vulkan_CommandBuffer::~Vulkan_CommandBuffer() {
		if (!is_valid())
			return;
		CATCH_SIGNAL(vkFreeCommandBuffers(vk_hDevice, vk_hCommandPool, 1U, &vk_hCommandBuffer));
	}

	void Vulkan_CommandBuffer::reset_command_buffer(const VkCommandBufferResetFlags vk_eCommandBufferResetFlags) const {
		CATCH_SIGNAL(vkResetCommandBuffer(vk_hCommandBuffer, vk_eCommandBufferResetFlags));
	}

	bool Vulkan_CommandBuffer::begin_recording_command_buffer(const VkCommandBufferUsageFlags vk_eCommandBufferUsageFlags) const {
		VkCommandBufferBeginInfo vk_commandBufferBeginInfo = {};
		vk_commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vk_commandBufferBeginInfo.flags = vk_eCommandBufferUsageFlags;
		if (!CHECK_VK_RESULT(vkBeginCommandBuffer(vk_hCommandBuffer, &vk_commandBufferBeginInfo))) {
			RE_ERROR("Failed to begin recording commands in Vulkan command buffer");
			return false;
		}
		return true;
	}

	bool Vulkan_CommandBuffer::end_recording_command_buffer() const {
		const bool bResult = CHECK_VK_RESULT(vkEndCommandBuffer(vk_hCommandBuffer));
		if (!bResult)
			RE_ERROR("Failed to finish recording Vulkan command buffer");
		return bResult;
	}

	void Vulkan_CommandBuffer::cmd_begin_renderpass(const VkRenderPassBeginInfo vk_commandBufferRenderpassBeginInfo, const VkSubpassContents vk_eSubpassContents) const {
		CATCH_SIGNAL(vkCmdBeginRenderPass(vk_hCommandBuffer, &vk_commandBufferRenderpassBeginInfo, vk_eSubpassContents));
	}

	void Vulkan_CommandBuffer::cmd_begin_renderpass(const float fClearColor[4], const int32_t i32ClearColor[4], const uint32_t u32ClearColor[4], const float fClearDepth, const uint32_t u32ClearStencil, const Vulkan_RenderPass *pRenderPass, const Vulkan_Framebuffer *pFramebuffer, const VkRect2D vk_renderArea, const VkSubpassContents vk_eSubpassContents) const {
		VkClearValue vk_clearValues;
		for (uint8_t u8ColorChannelIndex = 0U; u8ColorChannelIndex < 4U; u8ColorChannelIndex++) {
			vk_clearValues.color.float32[u8ColorChannelIndex] = fClearColor[u8ColorChannelIndex];
			vk_clearValues.color.int32[u8ColorChannelIndex] = i32ClearColor[u8ColorChannelIndex];
			vk_clearValues.color.uint32[u8ColorChannelIndex] = u32ClearColor[u8ColorChannelIndex];
		}
		vk_clearValues.depthStencil.depth = fClearDepth;
		vk_clearValues.depthStencil.stencil = u32ClearStencil;
		VkRenderPassBeginInfo vk_commandBufferRenderpassBeginInfo = {};
		vk_commandBufferRenderpassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		vk_commandBufferRenderpassBeginInfo.renderPass = *pRenderPass;
		vk_commandBufferRenderpassBeginInfo.framebuffer = *pFramebuffer;
		vk_commandBufferRenderpassBeginInfo.renderArea = vk_renderArea;
		vk_commandBufferRenderpassBeginInfo.clearValueCount = 1U;
		vk_commandBufferRenderpassBeginInfo.pClearValues = &vk_clearValues;
		CATCH_SIGNAL(this->cmd_begin_renderpass(vk_commandBufferRenderpassBeginInfo, vk_eSubpassContents));
	}

	void Vulkan_CommandBuffer::cmd_end_renderpass() const {
		CATCH_SIGNAL(vkCmdEndRenderPass(vk_hCommandBuffer));
	}

	void Vulkan_CommandBuffer::cmd_bind_pipeline(const VkPipelineBindPoint vk_ePipelineBindPoint, const VkPipeline vk_pipeline) const {
		CATCH_SIGNAL(vkCmdBindPipeline(vk_hCommandBuffer, vk_ePipelineBindPoint, vk_pipeline));
	}

	void Vulkan_CommandBuffer::cmd_bind_graphics_pipeline(const Vulkan_GraphicsPipeline *pGraphicsPipeline) const {
		CATCH_SIGNAL(cmd_bind_pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, *pGraphicsPipeline));
	}

	void Vulkan_CommandBuffer::cmd_bind_index_buffer(const VkBuffer vk_indexBuffer, const VkIndexType vk_eIndexDatatype) const {
		CATCH_SIGNAL(vkCmdBindIndexBuffer(vk_hCommandBuffer, vk_indexBuffer, 0U, vk_eIndexDatatype));
	}

	void Vulkan_CommandBuffer::cmd_bind_index_buffer(const Vulkan_Buffer *pIndexBuffer, const VkIndexType vk_eIndexDatatype) const {
		CATCH_SIGNAL(cmd_bind_index_buffer(*pIndexBuffer, vk_eIndexDatatype));
	}

	void Vulkan_CommandBuffer::cmd_bind_vertex_buffer(const VkBuffer *vk_pVertexBuffers, const VkDeviceSize *vk_pOffsets) const {
		CATCH_SIGNAL(vkCmdBindVertexBuffers(vk_hCommandBuffer, 0U, 1U, vk_pVertexBuffers, vk_pOffsets));
	}

	void Vulkan_CommandBuffer::cmd_bind_vertex_buffer(const Vulkan_Buffer *pVertexBuffer, const VkDeviceSize vk_offset) const {
		VkBuffer vk_vertexBuffers[1] = {*pVertexBuffer};
		VkDeviceSize vk_offsets[1] = {vk_offset};
		CATCH_SIGNAL(cmd_bind_vertex_buffer(vk_vertexBuffers, vk_offsets));
	}

	void Vulkan_CommandBuffer::cmd_copy_buffer(const VkBuffer vk_srcBuffer, const VkDeviceSize vk_srcOffset, const VkBuffer dstBuffer, const VkDeviceSize vk_dstOffset, const VkDeviceSize vk_bufferSize) const {
		VkBufferCopy vk_bufferCopy = {};
		vk_bufferCopy.srcOffset = vk_srcOffset;
		vk_bufferCopy.dstOffset = vk_dstOffset;
		vk_bufferCopy.size = vk_bufferSize;
		CATCH_SIGNAL(vkCmdCopyBuffer(vk_hCommandBuffer, vk_srcBuffer, dstBuffer, 1U, &vk_bufferCopy));
	}

	void Vulkan_CommandBuffer::cmd_copy_buffer(const VkBuffer vk_srcBuffer, const VkBuffer dstBuffer, const VkDeviceSize vk_bufferSize) const {
		CATCH_SIGNAL(cmd_copy_buffer(vk_srcBuffer, 0UL, dstBuffer, 0UL, vk_bufferSize));
	}

	void Vulkan_CommandBuffer::cmd_copy_buffer(const Vulkan_Buffer *pSrcBuffer, const VkDeviceSize vk_srcOffset, const Vulkan_Buffer *pDstBuffer, const VkDeviceSize vk_dstOffset, const VkDeviceSize vk_bufferSize) const {
		CATCH_SIGNAL(cmd_copy_buffer(*pSrcBuffer, vk_srcOffset, *pDstBuffer, vk_dstOffset, vk_bufferSize));
	}
	
	void Vulkan_CommandBuffer::cmd_copy_buffer(const Vulkan_Buffer *pSrcBuffer, const Vulkan_Buffer *pDstBuffer, const VkDeviceSize vk_bufferSize) const {
		CATCH_SIGNAL(cmd_copy_buffer(*pSrcBuffer, 0UL, *pDstBuffer, 0UL, vk_bufferSize));
	}

	void Vulkan_CommandBuffer::cmd_set_viewport(const VkViewport vk_viewport) const {
		CATCH_SIGNAL(vkCmdSetViewport(vk_hCommandBuffer, 0U, 1U, &vk_viewport));
	}

	void Vulkan_CommandBuffer::cmd_set_viewport(const float fX, const float fY, const float fWidth, const float fHeight, const float fMinDepth, const float fMaxDepth) const {
		VkViewport vk_viewport = {};
		vk_viewport.x = fX;
		vk_viewport.y = fY;
		vk_viewport.width = fWidth;
		vk_viewport.height = fHeight;
		vk_viewport.minDepth = fMinDepth;
		vk_viewport.maxDepth = fMaxDepth;
		CATCH_SIGNAL(this->cmd_set_viewport(vk_viewport));
	}

	void Vulkan_CommandBuffer::cmd_set_scissor(const VkRect2D vk_scissorRect) const {
		CATCH_SIGNAL(vkCmdSetScissor(vk_hCommandBuffer, 0U, 1U, &vk_scissorRect));
	}

	void Vulkan_CommandBuffer::cmd_set_scissor(const VkOffset2D vk_scissorOffset, const VkExtent2D vk_scissorExtent) const {
		VkRect2D vk_scissorRect = {};
		vk_scissorRect.offset = vk_scissorOffset;
		vk_scissorRect.extent = vk_scissorExtent;
		CATCH_SIGNAL(this->cmd_set_scissor(vk_scissorRect));
	}
	
	void Vulkan_CommandBuffer::cmd_set_scissor(const int32_t i32X, const int32_t i32Y, const uint32_t u32Width, const uint32_t u32Height) const {
		VkOffset2D vk_scissorOffset = {};
		vk_scissorOffset.x = i32X;
		vk_scissorOffset.y = i32Y;
		VkExtent2D vk_scissorExtent = {};
		vk_scissorExtent.width = u32Width;
		vk_scissorExtent.height = u32Height;
		CATCH_SIGNAL(this->cmd_set_scissor(vk_scissorOffset, vk_scissorExtent));
	}

	void Vulkan_CommandBuffer::cmd_draw(const uint32_t u32VerticesToDrawCount, const uint32_t u32InstanceCount, const uint32_t u32FirstVertex, const uint32_t u32FirstInstance) const {
		CATCH_SIGNAL(vkCmdDraw(vk_hCommandBuffer, u32VerticesToDrawCount, u32InstanceCount, u32FirstVertex, u32FirstInstance));
	}

	void Vulkan_CommandBuffer::cmd_draw_indexed(const uint32_t u32IndicesToDrawCount, const uint32_t u32InstanceCount, const uint32_t u32FirstIndex, const uint32_t u32VertexOffset, const uint32_t u32FirstInstance) const {
		CATCH_SIGNAL(vkCmdDrawIndexed(vk_hCommandBuffer, u32IndicesToDrawCount, u32InstanceCount, u32FirstIndex, u32VertexOffset, u32FirstInstance));
	}

	void Vulkan_CommandBuffer::cmd_execute(const uint32_t u32CommandBufferCount, const VkCommandBuffer *vk_pCommandBuffers) const {
		CATCH_SIGNAL(vkCmdExecuteCommands(vk_hCommandBuffer, u32CommandBufferCount, vk_pCommandBuffers));
	}
	
	void Vulkan_CommandBuffer::cmd_execute(const VkCommandBuffer vk_commandBuffer) const {
		VkCommandBuffer vk_commandBuffers[] = {vk_commandBuffer};
		CATCH_SIGNAL(cmd_execute(1U, vk_commandBuffers));
	}

	void Vulkan_CommandBuffer::cmd_execute(const uint32_t u32CommandBufferCount, const Vulkan_CommandBuffer *pCommandBuffer) const {
		VkCommandBuffer *vk_phCommandBuffers = new VkCommandBuffer[u32CommandBufferCount];
		for (uint32_t u32CommandBufferIndex = 0U; u32CommandBufferIndex < u32CommandBufferCount; u32CommandBufferIndex++)
			vk_phCommandBuffers[u32CommandBufferIndex] = pCommandBuffer[u32CommandBufferIndex].get_command_buffer();
		CATCH_SIGNAL(cmd_execute(u32CommandBufferCount, vk_phCommandBuffers));
		delete[] vk_phCommandBuffers;
	}

	void Vulkan_CommandBuffer::cmd_execute(const Vulkan_CommandBuffer *pCommandBuffer) const {
		VkCommandBuffer vk_hCommandBuffers[1] = {*pCommandBuffer};
		CATCH_SIGNAL(cmd_execute(1U, vk_hCommandBuffers));
	}

	VkCommandBuffer Vulkan_CommandBuffer::get_command_buffer() const {
		return vk_hCommandBuffer;
	}

	bool Vulkan_CommandBuffer::is_valid() const {
		return vk_hCommandBuffer != VK_NULL_HANDLE;
	}

	Vulkan_CommandBuffer::operator VkCommandBuffer() const {
		return this->vk_hCommandBuffer;
	}



	bool alloc_vk_command_buffers(const VkCommandBufferLevel vk_eCommandBufferLevel, const Vulkan_CommandPool *pCommandPool, const uint32_t u32CommandBufferCount, Vulkan_CommandBuffer **ppCommandBuffers) {
		for (uint32_t i = 0U; i < u32CommandBufferCount; i++)
			ppCommandBuffers[i] = new Vulkan_CommandBuffer(pCommandPool);
		VkCommandBufferAllocateInfo vk_commandBufferAllocInfo = {};
		vk_commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		vk_commandBufferAllocInfo.level = vk_eCommandBufferLevel;
		vk_commandBufferAllocInfo.commandPool = *pCommandPool;
		vk_commandBufferAllocInfo.commandBufferCount = u32CommandBufferCount;
		VkCommandBuffer *const vk_phCommandBuffers = new VkCommandBuffer[u32CommandBufferCount] {};
		if (!CHECK_VK_RESULT(vkAllocateCommandBuffers(vk_hDevice, &vk_commandBufferAllocInfo, vk_phCommandBuffers))) {
			RE_ERROR("Failed to allocate multiple Vulkan command buffers at once");
			delete[] vk_phCommandBuffers;
			return false;
		}
		for (uint32_t i = 0U; i < u32CommandBufferCount; i++)
			ppCommandBuffers[i]->vk_hCommandBuffer = vk_phCommandBuffers[i];
		delete[] vk_phCommandBuffers;
		return true;
	}

	void free_vk_command_buffers(const Vulkan_CommandPool *pCommandPool, const uint32_t u32CommandBufferCount, Vulkan_CommandBuffer **ppCommandBuffers) {
		VkCommandBuffer *vk_phCommandBuffers = new VkCommandBuffer[u32CommandBufferCount];
		for (uint32_t u32CommandBufferIndex = 0U; u32CommandBufferIndex < u32CommandBufferCount; u32CommandBufferIndex++)
			vk_phCommandBuffers[u32CommandBufferIndex] = ppCommandBuffers[u32CommandBufferIndex]->get_command_buffer();
		CATCH_SIGNAL(vkFreeCommandBuffers(vk_hDevice, *pCommandPool, u32CommandBufferCount, vk_phCommandBuffers));
	}

}