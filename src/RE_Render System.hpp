#ifndef __RE_RENDER_SYSTEM_H__
#define __RE_RENDER_SYSTEM_H__

#include "RE_Vulkan_Device.hpp"

namespace RE {

#define RE_VK_MAX_SAMPLED_IMAGES 0x7FFF
	
	// Attributes initialized at beginning and rarely changed
	extern VkPhysicalDevice *vk_pahPhysicalDevicesAvailable;
	extern uint32_t u32PhysicalDevicesAvailableCount;
	extern uint32_t u32IndexToSelectedPhysicalDevice;
	extern VkPhysicalDeviceProperties vk_physicalDeviceProperties;
	extern VkPhysicalDeviceFeatures vk_physicalDeviceFeatures;
	extern VkPhysicalDeviceMemoryProperties vk_physicalDeviceMemoryProperties;
	extern VkSurfaceKHR vk_hSurface;
	extern VkSurfaceFormatKHR vk_surfaceFormatSelected;
	extern VkSwapchainKHR vk_hSwapchain;
	extern VkFormat vk_eSwapchainImageFormat;
	extern VkExtent2D vk_swapchainResolution;
	extern VkImage *vk_pahSwapchainImages;
	extern VkImageView *vk_pahSwapchainImageViews;
	extern uint32_t u32SwapchainImageCount;

	bool init_render_system();
	void destroy_render_system();
	bool refresh_swapchain();
	void mark_swapchain_dirty();
	void create_queue_create_infos(const float &fPriority, std::vector<VkDeviceQueueCreateInfo> &vk_rpaLogicalQueueCreateInfos);

	class VulkanTask final {
		private:
			std::shared_ptr<uint8_t[]> queueIndicesPerCommandBuffer;
			std::shared_ptr<VkCommandPool[]> commandPoolPerCommandBuffer;
			VkCommandBuffer *vk_pahCommandBuffers;
			VkSemaphore *vk_pahInternalSemaphores;
			std::shared_ptr<uint32_t[]> commandBufferIndicesPerFunction;

		public:
			std::function<void (VkCommandBuffer vk_hCommandBuffer)> *const paFunctions;
			const uint32_t u32FunctionsCount;

		private:
			uint32_t u32CommandBufferCount;
			uint8_t u8PresentQueueIndex;

		public:
			VulkanTask() = delete;
			VulkanTask(uint32_t u32FunctionsCount, const VkQueueFlagBits *vk_paeQueueTypePerFunctionRequiredInOrder, bool bRequiresPresenting);
			VulkanTask(const VulkanTask &rCopy);
			~VulkanTask();
			void submit(uint32_t u32SemaphoresToWaitForCount, const VkSemaphore *vk_pahSemaphoresToWaitFor, const VkPipelineStageFlags *vk_pahStagesToWaitAt, uint32_t u32SemaphoresToSignal, const VkSemaphore *vk_pahSemaphoresToSignal, VkFence vk_hFenceToSignal);
			void submit_and_present(uint32_t u32SemaphoresToWaitForCount, const VkSemaphore *vk_pahSemaphoresToWaitFor, const VkPipelineStageFlags *vk_pahStagesToWaitAt, const VkSemaphore *vk_phSwapchainSemaphore, VkFence vk_hFenceToSignal, const uint32_t *pu32SwapchainImageIndex, VkResult *vk_pePresentResult);
	};

}

#endif /* __RE_RENDER_SYSTEM_H__ */