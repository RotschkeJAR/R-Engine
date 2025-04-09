#ifndef __RE_RENDERING_COMMAND_BUFFER_H__
#define __RE_RENDERING_COMMAND_BUFFER_H__

#include "RE_Vulkan.hpp"
#include "RE_Rendering_Render Pass.hpp"
#include "RE_Rendering_Pipeline.hpp"

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
			Rendering_CommandBuffer(const Rendering_CommandPool *pCommandPool);
			Rendering_CommandBuffer(const VkCommandBufferLevel vk_eCommandBufferLevel, const Rendering_CommandPool *pCommandPool);
			~Rendering_CommandBuffer();
			void reset_command_buffer(const VkCommandBufferResetFlags vk_eCommandBufferResetFlags) const;
			bool begin_recording_command_buffer(const VkCommandBufferUsageFlags vk_eCommandBufferUsageFlags) const;
			bool end_recording_command_buffer() const;
			void cmd_end_renderpass() const;
			void cmd_bind_graphics_pipeline(const Rendering_GraphicsPipeline *pGraphicsPipeline) const;
			void cmd_draw(const uint32_t u32VertexCount, const uint32_t u32InstanceCount, const uint32_t u32FirstVertex, const uint32_t u32FirstInstance) const;
			VkCommandBuffer get_command_buffer() const;
			bool is_valid() const;

			operator VkCommandBuffer() const;

		friend bool alloc_vk_command_buffers(const VkCommandBufferLevel vk_eCommandBufferLevel, const Rendering_CommandPool *pCommandPool, const uint32_t u32CommandBufferCount, Rendering_CommandBuffer **ppCommandBuffers);
	};

	bool alloc_vk_command_buffers(const VkCommandBufferLevel vk_eCommandBufferLevel, const Rendering_CommandPool *pCommandPool, const uint32_t u32CommandBufferCount, Rendering_CommandBuffer **ppCommandBuffers);
	void free_vk_command_buffers(const Rendering_CommandPool *pCommandPool, const uint32_t u32CommandBufferCount, Rendering_CommandBuffer **ppCommandBuffers);

}

#endif /* __RE_RENDERING_COMMAND_BUFFER_H__ */
