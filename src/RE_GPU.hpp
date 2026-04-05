#ifndef __RE_GPU_H__
#define __RE_GPU_H__

#include "RE_GPU_Vulkan Instance.hpp"
#include "RE_GPU_Vulkan Device.hpp"
#include "RE_GPU_Synchronization2.hpp"
#include "RE_GPU_Constrains.hpp"

namespace RE {

	// Surface
	extern VkPresentModeKHR vk_ePresentModeVsync, vk_ePresentModeNoVsync;
	extern VkSurfaceKHR vk_hSurface;
	extern VkSurfaceCapabilitiesKHR vk_surfaceCapabilities;
	extern std::unique_ptr<VkSurfaceFormatKHR[]> surfaceFormatsAvailable;
	extern uint32_t u32SurfaceFormatsAvailableCount, u32IndexToSelectedSurfaceFormat;
	bool init_logical_gpu();
	void destroy_logical_gpu();

	// Physical Device
	struct PhysicalVulkanDeviceInfo final {
		VkPhysicalDevice vk_hPhysicalDevice;
		const char *pacName;
		VkPhysicalDeviceType vk_eType;
		int32_t i32Scoring;
	};
	extern std::unique_ptr<PhysicalVulkanDeviceInfo[]> physicalDevicesAvailable;
	extern uint32_t u32PhysicalDevicesAvailableCount, u32IndexToSelectedPhysicalDevice;
	void select_physical_vulkan_device(uint32_t u32PhysicalDeviceIndex);
#define SELECTED_PHYSICAL_VULKAN_DEVICE physicalDevicesAvailable[u32IndexToSelectedPhysicalDevice].vk_hPhysicalDevice
#define SELECTED_PHYSICAL_VULKAN_DEVICE_NAME physicalDevicesAvailable[u32IndexToSelectedPhysicalDevice].pacName
#define SELECTED_PHYSICAL_VULKAN_DEVICE_TYPE physicalDevicesAvailable[u32IndexToSelectedPhysicalDevice].vk_eType
#define SELECTED_PHYSICAL_VULKAN_DEVICE_SCORING physicalDevicesAvailable[u32IndexToSelectedPhysicalDevice].i32Scoring

	// Scheduler
#define RE_VK_LOGICAL_QUEUE_IGNORED std::numeric_limits<uint8_t>::max()
	extern std::unique_ptr<uint32_t[]> queueFamilyIndices;
	extern std::unique_ptr<VkQueue[]> vk_pahQueues;
	extern std::unique_ptr<VkQueueFlags[]> vk_paeQueueTypes;
	extern std::vector<bool> presentationAvailablePerQueue;
	extern uint8_t u8LogicalQueueCount;
	void create_queue_create_infos(const float *pfPriority, std::vector<VkDeviceQueueCreateInfo> &vk_rpaLogicalQueueCreateInfos);
	VkQueue get_present_queue(uint8_t u8PreferredQueueIndex);

	class VulkanQueueCollection final {
		public:
			std::unique_ptr<uint32_t[]> queueFamilyIndices;
			std::unique_ptr<uint8_t[]> logicalQueueIndices;
			VkSharingMode vk_eSharingMode;
			uint8_t u8QueueCount;

			VulkanQueueCollection() = default;

			VulkanQueueCollection(VulkanQueueCollection&&) = default;
			VulkanQueueCollection& operator =(VulkanQueueCollection&&) = default;

			VulkanQueueCollection(VulkanQueueCollection&) = delete;
			VulkanQueueCollection& operator =(VulkanQueueCollection&) = delete;
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
			VulkanTask(VulkanTask&) = delete;
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

	// Memory
	struct SharedVulkanMemoryInfo final {
		std::variant<VkImage, VkBuffer> storageObject;
		VkMemoryPropertyFlags vk_mMemoryProperties;
		uint64_t m64CollidableMask;
		uint8_t u8OccupationIndex;
	};
	bool do_memory_properties_exist(VkMemoryPropertyFlags vk_mProperties, uint8_t *pu8Mismatches);
	bool is_staging_before_gpu_use_necessary();
	uint32_t get_remaining_vulkan_allocations();
	class VulkanMemory final {
		private:
			VkDeviceMemory vk_hMemory;
			VkDeviceSize vk_size;
			uint8_t u8MemoryType;
			bool bCoherent;

		public:
			VulkanMemory();
			VulkanMemory(VkDeviceSize vk_size, VkMemoryPropertyFlags vk_mProperties, uint32_t m32DesiredMemoryTypes);
			VulkanMemory(VkDeviceSize vk_size, uint8_t u8MemoryTypeIndex);
			VulkanMemory(VulkanMemory &rMemory) = delete;
			explicit VulkanMemory(VulkanMemory &&rrMemory);
			~VulkanMemory();
			
			VkResult alloc(VkDeviceSize vk_size, VkMemoryPropertyFlags vk_mProperties, uint32_t m32DesiredMemoryTypes);
			VkResult alloc(VkDeviceSize vk_size, uint8_t u8MemoryTypeIndex);
			VkResult alloc_for_buffer(VkBuffer vk_hBuffer, const VkMemoryPropertyFlags vk_mProperties);
			VkResult alloc_for_image(VkImage vk_hImage, const VkMemoryPropertyFlags vk_mProperties);
			void free();
			bool map(VkMemoryMapFlags vk_eFlags, VkDeviceSize vk_offset, VkDeviceSize vk_size, void **ppData) const;
			void unmap();

			bool valid() const;
			VkDeviceMemory get() const;
			VkDeviceSize size() const;
			uint8_t type_index() const;
			bool cpu_coherent() const;

			VulkanMemory& operator =(VulkanMemory &&rrMemory);
			VulkanMemory& operator =(VulkanMemory&) = delete;
			operator bool() const;
			VkDeviceMemory operator()() const;
			constexpr bool operator ==(const VulkanMemory &rOtherMemory) const {
				return false;
			}
			constexpr bool operator !=(const VulkanMemory &rOtherMemory) const {
				return true;
			}
	};

}

#endif /* __RE_GPU_H__ */
