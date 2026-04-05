#ifndef __RE_VULKAN_WRAPPERS_H__
#define __RE_VULKAN_WRAPPERS_H__

#include "RE_GPU.hpp"

namespace RE {

	bool create_vulkan_shader_from_file(const char *pacDirectory, VkShaderModuleCreateFlags vk_eFlags, VkShaderModule *vk_phShaderModule);
	
	bool create_vulkan_buffer(VkBufferCreateFlags vk_eFlags,
			VkDeviceSize vk_size,
			VkBufferUsageFlags vk_eUsages,
			uint32_t u32QueueFamilyCount,
			const uint32_t *pau32QueueFamilies,
			VkBuffer *vk_phBuffer);
	class Vulkan_Buffer final {
		private:
			VkBuffer vk_hBuffer;
			VulkanMemory memory;

		public:
			Vulkan_Buffer();
			Vulkan_Buffer(VkBufferCreateFlags vk_eFlags,
					VkDeviceSize vk_size,
					VkBufferUsageFlags vk_eUsages,
					uint32_t u32QueueCount,
					const uint32_t *pau32Queues,
					VkMemoryPropertyFlags vk_eMemoryPropertyFlags);
			Vulkan_Buffer(Vulkan_Buffer &rCopy) = delete;
			Vulkan_Buffer(Vulkan_Buffer &&rrCopy);
			~Vulkan_Buffer();

			bool create(VkBufferCreateFlags vk_eFlags,
					VkDeviceSize vk_size,
					VkBufferUsageFlags vk_eUsages,
					uint32_t u32QueueCount,
					const uint32_t *pau32Queues,
					VkMemoryPropertyFlags vk_eMemoryPropertyFlags);
			void destroy();

			VkBuffer get() const;
			const VkBuffer* get_ptr() const;
			VulkanMemory& get_memory();
			bool valid() const;

			operator VkBuffer() const;
			operator const VkBuffer*() const;
			operator VulkanMemory&();
			operator bool() const;
			VkBuffer operator()() const;
	};

	bool create_vulkan_buffer_view(VkBufferViewCreateFlags vk_eFlags,
			VkBuffer vk_hBuffer,
			VkFormat vk_eFormat,
			VkDeviceSize vk_offset,
			VkDeviceSize vk_range,
			VkBufferView *vk_phBufferView);
	class Vulkan_BufferView final {
		private:
			VkBufferView vk_hBufferView;

		public:
			Vulkan_BufferView();
			Vulkan_BufferView(VkBufferViewCreateFlags vk_eFlags,
					VkBuffer vk_hBuffer,
					VkFormat vk_eFormat,
					VkDeviceSize vk_offset,
					VkDeviceSize vk_range);
			Vulkan_BufferView(Vulkan_BufferView &rCopy) = delete;
			Vulkan_BufferView(Vulkan_BufferView &&rrCopy);
			~Vulkan_BufferView();

			bool create(VkBufferViewCreateFlags vk_eFlags,
					VkBuffer vk_hBuffer,
					VkFormat vk_eFormat,
					VkDeviceSize vk_offset,
					VkDeviceSize vk_range);
			void destroy();

			VkBufferView get() const;
			const VkBufferView* get_ptr() const;
			bool valid() const;

			operator VkBufferView() const;
			operator const VkBufferView*() const;
			operator bool() const;
			VkBufferView operator()() const;
	};

	bool create_vulkan_image(VkImageCreateFlags vk_eFlags,
			VkImageType vk_eType,
			VkFormat vk_eFormat,
			const VkExtent3D &vk_rExtent,
			uint32_t u32MipmapCount,
			uint32_t u32LayerCount,
			VkSampleCountFlagBits vk_eSamples,
			VkImageTiling vk_eTiling,
			VkImageUsageFlags vk_eUsage,
			uint32_t u32QueueFamilyCount,
			const uint32_t *pau32QueueFamilies,
			VkImageLayout vk_eInitialLayout,
			VkImage *vk_phImage);
	class Vulkan_Image final {
		private:
			VkImage vk_hImage;
			VulkanMemory memory;

		public:
			Vulkan_Image();
			Vulkan_Image(VkImageCreateFlags vk_eFlags,
					VkImageType vk_eType,
					VkFormat vk_eFormat,
					const VkExtent3D &vk_rExtent,
					uint32_t u32MipmapCount,
					uint32_t u32LayerCount,
					VkSampleCountFlagBits vk_eSamples,
					VkImageTiling vk_eTiling,
					VkImageUsageFlags vk_eUsage,
					uint32_t u32QueueFamilyCount,
					const uint32_t *pau32QueueFamilies,
					VkImageLayout vk_eInitialLayout,
					VkMemoryPropertyFlags vk_eMemoryProperties);
			Vulkan_Image(Vulkan_Image &rCopy) = delete;
			Vulkan_Image(Vulkan_Image &&rrCopy);
			~Vulkan_Image();

			bool create(VkImageCreateFlags vk_eFlags,
					VkImageType vk_eType,
					VkFormat vk_eFormat,
					const VkExtent3D &vk_rExtent,
					uint32_t u32MipmapCount,
					uint32_t u32LayerCount,
					VkSampleCountFlagBits vk_eSamples,
					VkImageTiling vk_eTiling,
					VkImageUsageFlags vk_eUsage,
					uint32_t u32QueueFamilyCount,
					const uint32_t *pau32QueueFamilies,
					VkImageLayout vk_eInitialLayout,
					VkMemoryPropertyFlags vk_eMemoryProperties);
			void destroy();

			VkImage get() const;
			const VkImage* get_ptr() const;
			VulkanMemory& get_memory();
			bool valid() const;

			operator VkImage() const;
			operator const VkImage*() const;
			operator VulkanMemory&();
			operator bool() const;
			VkImage operator()() const;
	};

	bool create_vulkan_image_view(VkImageViewCreateFlags vk_eFlags,
			VkImage vk_hImage,
			VkImageViewType vk_eType,
			VkFormat vk_eFormat,
			const VkComponentMapping &vk_rComponentMapping,
			const VkImageSubresourceRange &vk_rSubresourceRange,
			VkImageView *vk_phImageView);
	class Vulkan_ImageView final {
		private:
			VkImageView vk_hImageView;

		public:
			Vulkan_ImageView();
			Vulkan_ImageView(VkImageViewCreateFlags vk_eFlags,
					VkImage vk_hImage,
					VkImageViewType vk_eType,
					VkFormat vk_eFormat,
					const VkComponentMapping &vk_rComponentMapping,
					const VkImageSubresourceRange &vk_rSubresourceRange);
			Vulkan_ImageView(Vulkan_ImageView &rCopy) = delete;
			Vulkan_ImageView(Vulkan_ImageView &&rrCopy);
			~Vulkan_ImageView();

			bool create(VkImageViewCreateFlags vk_eFlags,
					VkImage vk_hImage,
					VkImageViewType vk_eType,
					VkFormat vk_eFormat,
					const VkComponentMapping &vk_rComponentMapping,
					const VkImageSubresourceRange &vk_rSubresourceRange);
			void destroy();

			VkImageView get() const;
			const VkImageView* get_ptr() const;
			bool valid() const;

			operator VkImageView() const;
			operator const VkImageView*() const;
			operator bool() const;
			VkImageView operator()() const;
	};

	bool create_vulkan_framebuffer(VkFramebufferCreateFlags vk_eFlags,
			VkRenderPass vk_hRenderPass,
			uint32_t u32AttachmentCount,
			const VkImageView *vk_pahAttachments,
			uint32_t u32Width,
			uint32_t u32Height,
			uint32_t u32Layers,
			VkFramebuffer *vk_phFramebuffer);
	class Vulkan_Framebuffer final {
		private:
			VkFramebuffer vk_hFramebuffer;

		public:
			Vulkan_Framebuffer();
			Vulkan_Framebuffer(VkFramebufferCreateFlags vk_eFlags,
					VkRenderPass vk_hRenderPass,
					uint32_t u32AttachmentCount,
					const VkImageView *vk_pahAttachments,
					uint32_t u32Width,
					uint32_t u32Height,
					uint32_t u32Layers);
			Vulkan_Framebuffer(Vulkan_Framebuffer &rCopy) = delete;
			Vulkan_Framebuffer(Vulkan_Framebuffer &&rrCopy);
			~Vulkan_Framebuffer();

			bool create(VkFramebufferCreateFlags vk_eFlags,
					VkRenderPass vk_hRenderPass,
					uint32_t u32AttachmentCount,
					const VkImageView *vk_pahAttachments,
					uint32_t u32Width,
					uint32_t u32Height,
					uint32_t u32Layers);
			void destroy();

			VkFramebuffer get() const;
			const VkFramebuffer* get_ptr() const;
			bool valid() const;

			operator VkFramebuffer() const;
			operator const VkFramebuffer*() const;
			operator bool() const;
			VkFramebuffer operator()() const;
	};

	bool alloc_vulkan_command_buffers(VkCommandPool vk_hCommandPool,
			VkCommandBufferLevel vk_eLevel,
			uint32_t u32Count,
			VkCommandBuffer *vk_pahCommandBuffers);
	bool begin_recording_vulkan_command_buffer(VkCommandBuffer vk_hCommandBuffer,
			VkCommandBufferUsageFlags vk_eUsage,
			const VkCommandBufferInheritanceInfo* vk_pInheritance);

	bool create_vulkan_fence(VkFenceCreateFlags vk_eFlags, VkFence *vk_phFence);
	class Vulkan_Fence final {
		private:
			VkFence vk_hFence;

		public:
			Vulkan_Fence();
			Vulkan_Fence(VkFenceCreateFlags vk_eCreateFlags);
			Vulkan_Fence(Vulkan_Fence &rCopy) = delete;
			Vulkan_Fence(Vulkan_Fence &&rrCopy);
			~Vulkan_Fence();

			bool create(VkFenceCreateFlags vk_eFlags);
			void destroy();
			VkResult wait_for(uint64_t u64Timeout = std::numeric_limits<uint64_t>::max()) const;
			void reset() const;

			VkFence get() const;
			const VkFence* get_ptr() const;
			bool valid() const;

			operator VkFence() const;
			operator const VkFence*() const;
			operator bool() const;
			VkFence operator()() const;
	};

	bool create_vulkan_semaphore(VkSemaphoreCreateFlags vk_eFlags, VkSemaphore *vk_phSemaphore);
	class Vulkan_Semaphore final {
		private:
			VkSemaphore vk_hSemaphore;

		public:
			Vulkan_Semaphore();
			Vulkan_Semaphore(VkSemaphoreCreateFlags vk_eFlags);
			Vulkan_Semaphore(Vulkan_Semaphore &rCopy) = delete;
			Vulkan_Semaphore(Vulkan_Semaphore &&rrCopy);
			~Vulkan_Semaphore();

			bool create(VkSemaphoreCreateFlags vk_eFlags);
			void destroy();

			VkSemaphore get() const;
			const VkSemaphore* get_ptr() const;
			bool valid() const;

			operator VkSemaphore() const;
			operator const VkSemaphore*() const;
			operator bool() const;
			VkSemaphore operator()() const;
	};

	bool create_vulkan_timeline_semaphore(VkSemaphoreCreateFlags vk_eFlags, uint64_t u64InitialValue, VkSemaphore *vk_phSemaphore);
	class Vulkan_TimelineSemaphore final {
		private:
			VkSemaphore vk_hTimelineSemaphore;

		public:
			Vulkan_TimelineSemaphore();
			Vulkan_TimelineSemaphore(VkSemaphoreCreateFlags vk_eFlags, uint64_t u64InitialValue = 0);
			Vulkan_TimelineSemaphore(Vulkan_TimelineSemaphore &rCopy) = delete;
			Vulkan_TimelineSemaphore(Vulkan_TimelineSemaphore &&rrCopy);
			~Vulkan_TimelineSemaphore();

			bool create(VkSemaphoreCreateFlags vk_eFlags, uint64_t u64InitialValue = 0);
			void destroy();
			bool wait_for_reaching(uint64_t u64Value) const;
			void set_to(uint64_t u64Value) const;

			VkSemaphore get() const;
			const VkSemaphore* get_ptr() const;
			bool valid() const;

			operator VkSemaphore() const;
			operator const VkSemaphore*() const;
			operator bool() const;
			VkSemaphore operator()() const;
	};

}

#endif /* __RE_VULKAN_WRAPPERS_H__ */
