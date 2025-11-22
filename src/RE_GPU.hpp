#ifndef __RE_GPU_H__
#define __RE_GPU_H__

#include "RE_Vulkan_Device.hpp"

namespace RE {

	extern std::unique_ptr<VkSurfaceFormatKHR[]> vk_paSurfaceFormatsAvailable;
	extern uint32_t u32IndexToSelectedSurfaceFormat;
	extern bool bSwapchainDirty;

	// Physical Device
	extern VkPhysicalDevice vk_hPhysicalDeviceSelected;
	extern VkPhysicalDeviceProperties vk_physicalDeviceProperties;
	extern VkPhysicalDeviceFeatures vk_physicalDeviceFeatures;
	extern VkPhysicalDeviceMemoryProperties vk_physicalDeviceMemoryProperties;

	// Scheduler
#define RE_VK_LOGICAL_QUEUE_IGNORED std::numeric_limits<uint8_t>::max()
	extern std::unique_ptr<uint32_t[]> queueFamilyIndices;
	extern uint8_t u8LogicalQueueCount;
	void create_queue_create_infos(const float *pfPriority, std::vector<VkDeviceQueueCreateInfo> &vk_rpaLogicalQueueCreateInfos);
	
	struct VulkanTask_Queues final {
		const uint8_t *pau8LogicalQueueIndices;
		const VkQueueFlagBits *vk_paeQueueTypes;
		const uint32_t *pau32StrictSeparationIds;
		uint32_t u32FunctionsCount;
	};

	class VulkanTask final {
		private:
			std::shared_ptr<uint8_t[]> queueIndicesPerCommandPool;
			std::unique_ptr<VkCommandPool[]> commandPools;
			std::shared_ptr<uint8_t[]> commandPoolIndexPerCommandBuffer;
			std::unique_ptr<VkCommandBuffer[]> commandBuffers;
			VkSemaphore vk_hInternalSemaphore;
			uint32_t u32FunctionsCount;
			uint8_t u8CommandPoolCount;
			bool bTransient;

		public:
			std::unique_ptr<std::function<void (VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue)>[]> paFunctions;

			VulkanTask();
			VulkanTask(uint32_t u32FunctionsCount, const VkQueueFlagBits *vk_paeQueueTypePerFunctionRequiredInOrder, bool bIndividualResets, bool bTransient);
			VulkanTask(uint32_t u32FunctionsCount, const uint8_t *pau8LogicalQueueIndexPerFunctionRequiredInOrder, bool bIndividualResets, bool bTransient);
			VulkanTask(const VulkanTask_Queues &rQueues, bool bIndividualResets, bool bTransient);
			VulkanTask(const VulkanTask &rCopy, bool bIndividualResets, bool bTransient);
			~VulkanTask();
			bool init(uint32_t u32FunctionsCount, const VkQueueFlagBits *vk_paeQueueTypePerFunctionRequiredInOrder, bool bIndividualResets, bool bTransient);
			bool init(uint32_t u32FunctionsCount, const uint8_t *pau8LogicalQueueIndexPerFunctionRequiredInOrder, bool bIndividualResets, bool bTransient);
			bool init(const VulkanTask_Queues &rQueues, bool bIndividualResets, bool bTransient);
			bool init(const VulkanTask &rCopy, bool bIndividualResets, bool bTransient);
			void destroy();
			void record(uint32_t u32FunctionIndex, VkCommandBufferUsageFlags vk_eUsageFlags, std::function<void (VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue)> pRecorderFunction);
			bool submit(uint32_t u32SemaphoresToWaitForCount, const VkSemaphoreSubmitInfo *vk_paSemaphoresToWaitFor, const VkPipelineStageFlags2 *vk_paeInternSemaphoreWaits, uint32_t u32SemaphoresToSignal, const VkSemaphoreSubmitInfo *vk_paSemaphoresToSignal, VkFence vk_hFenceToSignal);
			void reset_all(VkCommandPoolResetFlags vk_eResetFlags) const;
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