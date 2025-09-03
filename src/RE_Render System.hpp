#ifndef __RE_RENDER_SYSTEM_H__
#define __RE_RENDER_SYSTEM_H__

#include "RE_Vulkan_Device.hpp"

namespace RE {

	// Physical Device
	extern VkPhysicalDevice vk_hPhysicalDeviceSelected;
	extern VkPhysicalDeviceProperties vk_physicalDeviceProperties;
	extern VkPhysicalDeviceFeatures vk_physicalDeviceFeatures;
	extern VkPhysicalDeviceMemoryProperties vk_physicalDeviceMemoryProperties;

	// Scheduler
	void create_queue_create_infos(const float *pfPriority, std::vector<VkDeviceQueueCreateInfo> &vk_rpaLogicalQueueCreateInfos);

	// Swapchain
	extern VkSwapchainKHR vk_hSwapchain;
	extern VkFormat vk_eSwapchainImageFormat;
	extern VkExtent2D vk_swapchainResolution;
	extern std::unique_ptr<VkImage[]> vk_pahSwapchainImages;
	extern std::unique_ptr<VkImageView[]> vk_pahSwapchainImageViews;
	extern uint32_t u32SwapchainImageCount;

	bool init_render_system();
	void destroy_render_system();
	bool refresh_swapchain();
	void mark_swapchain_dirty();

	class VulkanTask final {
		private:
			std::shared_ptr<uint8_t[]> queueIndicesPerCommandBuffer;
			std::shared_ptr<VkCommandPool[]> commandPoolPerCommandBuffer;
			std::unique_ptr<VkCommandBuffer[]> vk_pahCommandBuffers;
			std::unique_ptr<VkSemaphore[]> vk_pahInternalSemaphores;
			std::shared_ptr<uint32_t[]> commandBufferIndicesPerFunction;
			uint32_t u32FunctionsCount;

		public:
			std::unique_ptr<std::function<void (VkCommandBuffer vk_hCommandBuffer, uint32_t u32PreviousQueueFamily, uint32_t u32CurrentQueueFamily, uint32_t u32NextQueueFamily)>[]> paFunctions;

		private:
			uint32_t u32CommandBufferCount;
			uint8_t u8PresentQueueIndex;

		public:
			VulkanTask();
			VulkanTask(uint32_t u32FunctionsCount, const VkQueueFlagBits *vk_paeQueueTypePerFunctionRequiredInOrder, bool bRequiresPresenting, bool bTransient);
			VulkanTask(const VulkanTask &rCopy);
			~VulkanTask();
			void init(uint32_t u32FunctionsCount, const VkQueueFlagBits *vk_paeQueueTypePerFunctionRequiredInOrder, bool bRequiresPresenting, bool bTransient);
			void init(const VulkanTask &rCopy);
			void destroy();
			void record(VkCommandBufferUsageFlags vk_eUsageFlags);
			void submit(uint32_t u32SemaphoresToWaitForCount, const VkSemaphore *vk_pahSemaphoresToWaitFor, const VkPipelineStageFlags *vk_pahStagesToWaitAt, uint32_t u32SemaphoresToSignal, const VkSemaphore *vk_pahSemaphoresToSignal, VkFence vk_hFenceToSignal);
			void submit_and_present(uint32_t u32SemaphoresToWaitForCount, const VkSemaphore *vk_pahSemaphoresToWaitFor, const VkPipelineStageFlags *vk_pahStagesToWaitAt, const VkSemaphore *vk_phSwapchainSemaphore, VkFence vk_hFenceToSignal, const uint32_t *pu32SwapchainImageIndex, VkResult *vk_pePresentResult);
			uint32_t get_function_count() const;
			uint32_t get_queue_family_index_for_function(uint32_t u32FunctionIndex) const;
	};

}

#endif /* __RE_RENDER_SYSTEM_H__ */