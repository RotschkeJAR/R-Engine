#ifndef __RE_GPU_H__
#define __RE_GPU_H__

#include "RE_Vulkan_Device.hpp"

namespace RE {

	extern VkSurfaceKHR vk_hSurface;

	extern std::unique_ptr<VkSurfaceFormatKHR[]> vk_paSurfaceFormatsAvailable;
	extern uint32_t u32IndexToSelectedSurfaceFormat;
	extern bool bSwapchainDirty;

	// Physical Device
	extern VkPhysicalDevice vk_hPhysicalDeviceSelected;
	extern VkPhysicalDeviceProperties vk_physicalDeviceProperties;
	extern VkPhysicalDeviceFeatures vk_physicalDeviceFeatures;

	// Scheduler
#define RE_VK_LOGICAL_QUEUE_IGNORED std::numeric_limits<uint8_t>::max()
	extern std::unique_ptr<uint32_t[]> queueFamilyIndices;
	extern std::unique_ptr<VkQueue[]> vk_pahQueues;
	extern std::unique_ptr<VkQueueFlags[]> vk_paeQueueTypes;
	extern std::vector<bool> presentationAvailablePerQueue;
	extern uint8_t u8LogicalQueueCount;
	void create_queue_create_infos(const float *pfPriority, std::vector<VkDeviceQueueCreateInfo> &vk_rpaLogicalQueueCreateInfos);
	VkQueue get_present_queue(uint8_t u8PreferredQueueIndex);

	struct VulkanQueueCollection final {
		std::unique_ptr<uint32_t[]> queueFamilyIndices;
		std::unique_ptr<uint8_t[]> logicalQueueIndices;
		VkSharingMode vk_eSharingMode;
		uint8_t u8QueueCount;
	};
	
	struct VulkanTask_Queues final {
		const uint8_t *pau8LogicalQueueIndices;
		const VkQueueFlagBits *vk_paeQueueTypes;
		const uint32_t *pau32StrictSeparationIds;
		uint32_t u32FunctionsCount;
	};

	class VulkanTask final {
		private:
			std::shared_ptr<uint8_t[]> queueIndexPerCommandPool;
			std::unique_ptr<VkCommandPool[]> commandPools;
			std::shared_ptr<uint8_t[]> commandPoolIndexPerCommandBuffer;
			std::unique_ptr<VkCommandBuffer[]> commandBuffers;
			VkSemaphore vk_hInternalSemaphore;
			uint32_t u32FunctionsCount;
			uint8_t u8CommandPoolCount;
			uint8_t u8LogicalPresentQueueIndex;
			bool bTransient;

		public:
			VulkanTask();
			VulkanTask(uint32_t u32FunctionsCount, const VkQueueFlagBits *vk_paeQueueTypePerFunctionRequiredInOrder, bool bIndividualResets, bool bIncludePresentation, bool bTransient);
			VulkanTask(uint32_t u32FunctionsCount, const uint8_t *pau8LogicalQueueIndexPerFunctionRequiredInOrder, bool bIndividualResets, bool bIncludePresentation, bool bTransient);
			VulkanTask(const VulkanTask_Queues &rQueues, bool bIndividualResets, bool bIncludePresentation, bool bTransient);
			VulkanTask(const VulkanTask &rCopy, bool bIndividualResets, bool bTransient);
			VulkanTask(VulkanTask &&rrTask);
			~VulkanTask();
			bool init(uint32_t u32FunctionsCount, const VkQueueFlagBits *vk_paeQueueTypePerFunctionRequiredInOrder, bool bIndividualResets, bool bIncludePresentation, bool bTransient);
			bool init(uint32_t u32FunctionsCount, const uint8_t *pau8LogicalQueueIndexPerFunctionRequiredInOrder, bool bIndividualResets, bool bIncludePresentation, bool bTransient);
			bool init(const VulkanTask_Queues &rQueues, bool bIndividualResets, bool bIncludePresentation, bool bTransient);
			bool init(const VulkanTask &rCopy, bool bIndividualResets, bool bTransient);
			void destroy();
			bool record(uint32_t u32FunctionIndex, VkCommandBufferUsageFlags vk_eUsageFlags, std::function<void (VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue)> pRecorderFunction) const;
			bool submit(uint32_t u32SemaphoresToWaitForCount, const VkSemaphoreSubmitInfo *vk_paSemaphoresToWaitFor, const VkPipelineStageFlags2 *vk_paeInternSemaphoreWaits, uint32_t u32SemaphoresToSignal, const VkSemaphoreSubmitInfo *vk_paSemaphoresToSignal, VkFence vk_hFenceToSignal) const;
			void reset_all(VkCommandPoolResetFlags vk_eResetFlags) const;
			VkCommandPool command_pool_of_function(uint32_t u32FunctionIndex) const;
			uint32_t function_count() const;
			uint8_t logical_queue_index_for_function(uint32_t u32FunctionIndex) const;
			uint8_t logical_queue_index_for_presentation() const;
			VulkanQueueCollection queues_of_functions(const uint32_t *pau32FunctionIndices, uint32_t u32FunctionIndexCount) const;
			bool valid() const;
	};

	// Swapchain
	extern VkSwapchainKHR vk_hSwapchain;
	extern VkFormat vk_eSwapchainImageFormat;
	extern VkExtent2D vk_swapchainResolution;
	extern std::unique_ptr<VkImage[]> vk_pahSwapchainImages;
	extern std::unique_ptr<VkImageView[]> vk_pahSwapchainImageViews;
	extern uint32_t u32SwapchainImageCount;
	bool create_swapchain();
	void destroy_swapchain();

	bool init_render_system();
	void destroy_render_system();
	bool refresh_swapchain();
	void mark_swapchain_dirty();

	// Memory
	enum VulkanMemoryType {
		RE_VK_GPU_RAM,
		RE_VK_CPU_RAM,
		RE_VK_SHARED_RAM
	};
	bool does_memory_type_exist(VkMemoryPropertyFlags vk_eProperties);
	bool does_have_vulkan_memory_type(VulkanMemoryType eMemoryType);
	bool does_vulkan_memory_type_reside_on_cpu(uint8_t u8MemoryTypeIndex);
	bool does_vulkan_memory_type_reside_on_gpu(uint8_t u8MemoryTypeIndex);
	bool is_transfer_necessary();
	uint32_t get_remaining_vulkan_allocations();
	class VulkanMemory final {
		private:
			VkDeviceMemory vk_hMemory;
			VkDeviceSize vk_size;
			uint8_t u8MemoryTypeIndex;
			bool bCoherent;

		public:
			VulkanMemory();
			VulkanMemory(VkDeviceSize vk_size, VulkanMemoryType eType, VkMemoryPropertyFlags vk_eProperties, uint32_t b32DesiredMemoryTypes);
			VulkanMemory(VkDeviceSize vk_size, uint8_t u8MemoryTypeIndex);
			VulkanMemory(VulkanMemory &rMemory) = delete;
			explicit VulkanMemory(VulkanMemory &&rrMemory) noexcept;
			~VulkanMemory() noexcept;
			
			bool alloc(VkDeviceSize vk_size, VulkanMemoryType eType, VkMemoryPropertyFlags vk_eProperties, uint32_t b32DesiredMemoryTypes);
			bool alloc(VkDeviceSize vk_size, uint8_t u8MemoryTypeIndex);
			void free() noexcept;
			bool map(VkMemoryMapFlags vk_eFlags, VkDeviceSize vk_offset, VkDeviceSize vk_size, void **ppData) const;
			void unmap();

			bool valid() const;
			VkDeviceMemory get() const;
			VkDeviceSize size() const;
			uint8_t type_index() const;
			bool cpu_coherent() const;

			void operator =(VulkanMemory &&rrMemory) noexcept;
			constexpr bool operator ==(const VulkanMemory &rOtherMemory) const {
				return false;
			}
			constexpr bool operator !=(const VulkanMemory &rOtherMemory) const {
				return true;
			}
	};

}

#endif /* __RE_GPU_H__ */
