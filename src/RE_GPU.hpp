#ifndef __RE_GPU_H__
#define __RE_GPU_H__

#include "RE_Vulkan_Device.hpp"

namespace RE {

	extern std::unique_ptr<VkSurfaceFormatKHR[]> vk_paSurfaceFormatsAvailable;
	extern uint32_t u32IndexToSelectedSurfaceFormat;

	// Physical Device
	extern VkPhysicalDevice vk_hPhysicalDeviceSelected;
	extern VkPhysicalDeviceProperties vk_physicalDeviceProperties;
	extern VkPhysicalDeviceFeatures vk_physicalDeviceFeatures;
	extern VkPhysicalDeviceMemoryProperties vk_physicalDeviceMemoryProperties;

	// Scheduler
	extern std::unique_ptr<uint32_t[]> queueFamilyIndices;
	extern uint8_t u8LogicalQueueCount;
	void create_queue_create_infos(const float *pfPriority, std::vector<VkDeviceQueueCreateInfo> &vk_rpaLogicalQueueCreateInfos);

#define RE_VK_LOGICAL_QUEUE_IGNORED std::numeric_limits<uint8_t>::max()
	struct VulkanTask_Queues final {
		const uint8_t *pau8LogicalQueueIndices;
		const VkQueueFlagBits *vk_paeQueueTypes;
		const uint32_t *pau32StrictSeparationIds;
		uint32_t u32FunctionsCount;
	};
	class VulkanTask final {
		private:
			std::shared_ptr<uint8_t[]> queueIndicesPerCommandBuffer;
			std::shared_ptr<VkCommandPool[]> commandPoolPerCommandBuffer;
			std::unique_ptr<VkCommandBuffer[]> commandBuffers;
			std::shared_ptr<uint32_t[]> commandBufferIndicesPerFunction;
			VkSemaphore vk_hInternalSemaphore;
			uint32_t u32CommandBufferCount;

		public:
			uint32_t u32FunctionsCount;
			std::unique_ptr<std::function<void (VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue)>[]> paFunctions;

			VulkanTask();
			VulkanTask(uint32_t u32FunctionsCount, const VkQueueFlagBits *vk_paeQueueTypePerFunctionRequiredInOrder, bool bTransient);
			VulkanTask(uint32_t u32FunctionsCount, const uint8_t *pau8LogicalQueueIndexPerFunctionRequiredInOrder, bool bTransient);
			VulkanTask(const VulkanTask_Queues &rQueues, bool bTransient);
			VulkanTask(const VulkanTask &rCopy);
			~VulkanTask();
			bool init(uint32_t u32FunctionsCount, const VkQueueFlagBits *vk_paeQueueTypePerFunctionRequiredInOrder, bool bTransient);
			bool init(uint32_t u32FunctionsCount, const uint8_t *pau8LogicalQueueIndexPerFunctionRequiredInOrder, bool bTransient);
			bool init(const VulkanTask_Queues &rQueues, bool bTransient);
			bool init(const VulkanTask &rCopy);
			void destroy();
			void record(VkCommandBufferUsageFlags vk_eUsageFlags);
			bool submit(uint32_t u32SemaphoresToWaitForCount, const VkSemaphoreSubmitInfo *vk_paSemaphoresToWaitFor, uint32_t u32SemaphoresToSignal, const VkSemaphoreSubmitInfo *vk_paSemaphoresToSignal, VkFence vk_hFenceToSignal);
			uint32_t get_function_count() const;
			uint8_t get_logical_queue_index_for_function(uint32_t u32FunctionIndex) const;
			bool is_valid() const;
	};

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

}

#endif /* __RE_GPU_H__ */