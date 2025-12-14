#ifndef __RE_VULKAN_WRAPPER_CLASSES_H__
#define __RE_VULKAN_WRAPPER_CLASSES_H__

#include "RE_GPU.hpp"

namespace RE {

	bool create_vulkan_shader_from_file(const char *pacDirectory, VkShaderModuleCreateFlags vk_eFlags, VkShaderModule *vk_phShaderModule);
	
	bool create_vulkan_buffer(VkBufferCreateFlags vk_eFlags, 
			VkDeviceSize vk_size, 
			VkBufferUsageFlags vk_eUsages, 
			uint32_t u32QueueFamilyCount, 
			const uint32_t *pau32QueueFamilies, 
			VulkanMemoryType eMemoryType, 
			VkMemoryPropertyFlags vk_eMemoryPropertyFlags, 
			VkBuffer *vk_phBuffer, 
			VulkanMemory *pMemory);
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
					VulkanMemoryType eMemoryType, 
					VkMemoryPropertyFlags vk_eMemoryPropertyFlags);
			Vulkan_Buffer(Vulkan_Buffer &rCopy) = delete;
			Vulkan_Buffer(Vulkan_Buffer &&rrCopy);
			~Vulkan_Buffer();

			bool create(VkBufferCreateFlags vk_eFlags, 
					VkDeviceSize vk_size, 
					VkBufferUsageFlags vk_eUsages, 
					uint32_t u32QueueCount, 
					const uint32_t *pau32Queues, 
					VulkanMemoryType eMemoryType, 
					VkMemoryPropertyFlags vk_eMemoryPropertyFlags);
			void destroy();

			[[nodiscard]]
			VkBuffer get() const noexcept;
			[[nodiscard]]
			const VkBuffer* get_ptr() const noexcept;
			[[nodiscard]]
			VulkanMemory& get_memory() noexcept;
			[[nodiscard]]
			bool valid() const noexcept;

			[[nodiscard]]
			operator VkBuffer() const;
			[[nodiscard]]
			operator const VkBuffer*() const;
			[[nodiscard]]
			operator VulkanMemory&();
			[[nodiscard]]
			operator bool() const;
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

			[[nodiscard]]
			VkBufferView get() const noexcept;
			[[nodiscard]]
			const VkBufferView* get_ptr() const noexcept;
			[[nodiscard]]
			bool valid() const noexcept;

			[[nodiscard]]
			operator VkBufferView() const;
			[[nodiscard]]
			operator const VkBufferView*() const;
			[[nodiscard]]
			operator bool() const;
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
			VulkanMemoryType eMemoryType, 
			VkMemoryPropertyFlags vk_eMemoryProperties, 
			VkImage *vk_phImage, 
			VulkanMemory *pMemory);
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
					VulkanMemoryType eMemoryType, 
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
					VulkanMemoryType eMemoryType, 
					VkMemoryPropertyFlags vk_eMemoryProperties);
			void destroy();

			[[nodiscard]]
			VkImage get() const noexcept;
			[[nodiscard]]
			const VkImage* get_ptr() const noexcept;
			[[nodiscard]]
			VulkanMemory& get_memory() noexcept;
			[[nodiscard]]
			bool valid() const noexcept;

			[[nodiscard]]
			operator VkImage() const;
			[[nodiscard]]
			operator const VkImage*() const;
			[[nodiscard]]
			operator VulkanMemory&();
			[[nodiscard]]
			operator bool() const;
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

			[[nodiscard]]
			VkImageView get() const noexcept;
			[[nodiscard]]
			const VkImageView* get_ptr() const noexcept;
			[[nodiscard]]
			bool valid() const noexcept;

			[[nodiscard]]
			operator VkImageView() const;
			[[nodiscard]]
			operator const VkImageView*() const;
			[[nodiscard]]
			operator bool() const;
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

			[[nodiscard]]
			VkFramebuffer get() const noexcept;
			[[nodiscard]]
			const VkFramebuffer* get_ptr() const noexcept;
			[[nodiscard]]
			bool valid() const noexcept;

			[[nodiscard]]
			operator VkFramebuffer() const;
			[[nodiscard]]
			operator const VkFramebuffer*() const;
			[[nodiscard]]
			operator bool() const;
	};

	bool alloc_vulkan_command_buffers(
			VkCommandPool vk_hCommandPool, 
			VkCommandBufferLevel vk_eLevel, 
			uint32_t u32Count, 
			VkCommandBuffer *vk_pahCommandBuffers);
	bool begin_recording_vulkan_command_buffer(
			VkCommandBuffer vk_hCommandBuffer, 
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

			[[nodiscard]]
			VkFence get() const noexcept;
			[[nodiscard]]
			const VkFence* get_ptr() const noexcept;
			[[nodiscard]]
			bool valid() const noexcept;

			[[nodiscard]]
			operator VkFence() const;
			[[nodiscard]]
			operator const VkFence*() const;
			[[nodiscard]]
			operator bool() const;
	};

	class Vulkan_TimelineSemaphore final {
		private:
			VkSemaphore vk_hTimelineSemaphore;

		public:
			Vulkan_TimelineSemaphore();
			Vulkan_TimelineSemaphore(uint64_t u64InitialValue);
			~Vulkan_TimelineSemaphore();

			bool wait_for_reaching(uint64_t u64Value) const;
			void set_to(uint64_t u64Value) const;

			[[nodiscard]]
			VkSemaphore get() const noexcept;
			[[nodiscard]]
			const VkSemaphore* get_ptr() const noexcept;
			[[nodiscard]]
			bool valid() const noexcept;

			[[nodiscard]]
			operator VkSemaphore() const;
			[[nodiscard]]
			operator const VkSemaphore*() const;
			[[nodiscard]]
			operator bool() const;
	};

}

#endif /* __RE_VULKAN_WRAPPER_CLASSES_H__ */
