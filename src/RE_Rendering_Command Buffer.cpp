#include "RE_Rendering_Command Buffer.hpp"
#include "RE_Render System.hpp"

namespace RE {
	
	Rendering_CommandPool::Rendering_CommandPool(const VkCommandPoolCreateFlagBits vk_eCommandPoolCreateFlags, const uint32_t u32QueueFamilyIndex) : vk_hCommandPool(VK_NULL_HANDLE) {
		VkCommandPoolCreateInfo vk_commandPoolCreateInfo = {};
		vk_commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		vk_commandPoolCreateInfo.flags = vk_eCommandPoolCreateFlags;
		vk_commandPoolCreateInfo.queueFamilyIndex = u32QueueFamilyIndex;
		if (!CHECK_VK_RESULT(vkCreateCommandPool(vk_hDevice, &vk_commandPoolCreateInfo, nullptr, &vk_hCommandPool))) {
			RE_ERROR("Failed to create a Vulkan command pool");
			vk_hCommandPool = VK_NULL_HANDLE;
		}
	}

	Rendering_CommandPool::~Rendering_CommandPool() {
		if (!is_valid())
			return;
		CATCH_SIGNAL(vkDestroyCommandPool(vk_hDevice, vk_hCommandPool, nullptr));
	}

	VkCommandPool Rendering_CommandPool::get_command_pool() const {
		return vk_hCommandPool;
	}

	bool  Rendering_CommandPool::is_valid() const {
		return vk_hCommandPool != VK_NULL_HANDLE;
	}

	Rendering_CommandPool::operator VkCommandPool() const {
		return this->vk_hCommandPool;
	}


	
	Rendering_CommandBuffer::Rendering_CommandBuffer(const Rendering_CommandPool *pCommandPool) : vk_hCommandBuffer(VK_NULL_HANDLE), vk_hCommandPool(*pCommandPool) {}

	Rendering_CommandBuffer::Rendering_CommandBuffer(const VkCommandBufferLevel vk_eCommandBufferLevel, const Rendering_CommandPool *pCommandPool) : vk_hCommandBuffer(VK_NULL_HANDLE), vk_hCommandPool(*pCommandPool) {
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

	Rendering_CommandBuffer::~Rendering_CommandBuffer() {
		if (!is_valid())
			return;
		CATCH_SIGNAL(vkFreeCommandBuffers(vk_hDevice, vk_hCommandPool, 1U, &vk_hCommandBuffer));
	}

	void Rendering_CommandBuffer::reset_command_buffer(const VkCommandBufferResetFlags vk_eCommandBufferResetFlags) const {
		CATCH_SIGNAL(vkResetCommandBuffer(vk_hCommandBuffer, vk_eCommandBufferResetFlags));
	}

	bool Rendering_CommandBuffer::begin_recording_command_buffer(const VkCommandBufferUsageFlags vk_eCommandBufferUsageFlags) const {
		VkCommandBufferBeginInfo vk_commandBufferBeginInfo = {};
		vk_commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vk_commandBufferBeginInfo.flags = vk_eCommandBufferUsageFlags;
		if (!CHECK_VK_RESULT(vkBeginCommandBuffer(vk_hCommandBuffer, &vk_commandBufferBeginInfo))) {
			RE_ERROR("Failed to begin recording commands in Vulkan command buffer");
			return false;
		}
		return true;
	}

	bool Rendering_CommandBuffer::end_recording_command_buffer() const {
		const bool bResult = CATCH_SIGNAL_AND_RETURN(CHECK_VK_RESULT(vkEndCommandBuffer(vk_hCommandBuffer)), bool);
		if (!bResult)
			RE_ERROR("Failed to finish recording Vulkan command buffer");
		return bResult;
	}

	void Rendering_CommandBuffer::cmd_end_renderpass() const {
		CATCH_SIGNAL(vkCmdEndRenderPass(vk_hCommandBuffer));
	}

	void Rendering_CommandBuffer::cmd_bind_graphics_pipeline(const Rendering_GraphicsPipeline *pGraphicsPipeline) const {
		CATCH_SIGNAL(vkCmdBindPipeline(vk_hCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pGraphicsPipeline));
	}

	void Rendering_CommandBuffer::cmd_draw(const uint32_t u32VertexCount, const uint32_t u32InstanceCount, const uint32_t u32FirstVertex, const uint32_t u32FirstInstance) const {
		CATCH_SIGNAL(vkCmdDraw(vk_hCommandBuffer, u32VertexCount, u32InstanceCount, u32FirstVertex, u32FirstInstance));
	}

	VkCommandBuffer Rendering_CommandBuffer::get_command_buffer() const {
		return vk_hCommandBuffer;
	}

	bool Rendering_CommandBuffer::is_valid() const {
		return vk_hCommandBuffer != VK_NULL_HANDLE;
	}

	Rendering_CommandBuffer::operator VkCommandBuffer() const {
		return this->vk_hCommandBuffer;
	}



	bool alloc_vk_command_buffers(const VkCommandBufferLevel vk_eCommandBufferLevel, const Rendering_CommandPool *pCommandPool, const uint32_t u32CommandBufferCount, Rendering_CommandBuffer **ppCommandBuffers) {
		for (uint32_t i = 0U; i < u32CommandBufferCount; i++)
			ppCommandBuffers[i] = new Rendering_CommandBuffer(pCommandPool);
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

	void free_vk_command_buffers(const Rendering_CommandPool *pCommandPool, const uint32_t u32CommandBufferCount, Rendering_CommandBuffer **ppCommandBuffers) {
		VkCommandBuffer *vk_phCommandBuffers = new VkCommandBuffer[u32CommandBufferCount];
		for (uint32_t u32CommandBufferIndex = 0U; u32CommandBufferIndex < u32CommandBufferCount; u32CommandBufferIndex++)
			vk_phCommandBuffers[u32CommandBufferIndex] = ppCommandBuffers[u32CommandBufferIndex]->get_command_buffer();
		CATCH_SIGNAL(vkFreeCommandBuffers(vk_hDevice, *pCommandPool, u32CommandBufferCount, vk_phCommandBuffers));
	}

}