#ifndef __RE_GPU_H__
#define __RE_GPU_H__ 1

#include "RE_GPU_VulkanInstance.hpp"
#include "RE_GPU_VulkanDevice.hpp"
#include "RE_GPU_Synchronization2.hpp"
#include "RE_GPU_Constrains.hpp"

namespace RE {

#define PREFERRED_GPU_MEMORY_ALIGNMENT   16

	// Surface
	extern VkSurfaceKHR vk_hSurface;
	extern VkSurfaceCapabilitiesKHR vk_surfaceCapabilities;
	extern std::unique_ptr<VkSurfaceFormatKHR[]> std_surfaceFormatsAvailable;
	extern VkCompositeAlphaFlagBitsKHR vk_eCompositeAlphaSelected;
	extern uint32_t u32SurfaceFormatsAvailableCount,
		u32IndexToSelectedSurfaceFormat;
	extern VkPresentModeKHR vk_ePresentNoVsync,
		vk_ePresentVsync;
	bool init_logical_gpu();
	void destroy_logical_gpu();

	// Physical Device
	struct PhysicalVulkanDeviceInfo final {
		VkPhysicalDevice vk_hPhysicalDevice;
		const char *pacName;
		VkPhysicalDeviceType vk_eType;
		int32_t i32Scoring;
	};
	extern std::unique_ptr<PhysicalVulkanDeviceInfo[]> std_physicalDevicesAvailable;
	extern uint32_t u32PhysicalDevicesAvailableCount,
		u32IndexToSelectedPhysicalDevice;
#define SELECTED_PHYSICAL_VULKAN_DEVICE           std_physicalDevicesAvailable[u32IndexToSelectedPhysicalDevice].vk_hPhysicalDevice
#define SELECTED_PHYSICAL_VULKAN_DEVICE_NAME      std_physicalDevicesAvailable[u32IndexToSelectedPhysicalDevice].pacName
#define SELECTED_PHYSICAL_VULKAN_DEVICE_TYPE      std_physicalDevicesAvailable[u32IndexToSelectedPhysicalDevice].vk_eType
#define SELECTED_PHYSICAL_VULKAN_DEVICE_SCORING   std_physicalDevicesAvailable[u32IndexToSelectedPhysicalDevice].i32Scoring
	void select_physical_vulkan_device(uint32_t u32PhysicalDeviceIndex);

	// Scheduler
#define RE_VK_LOGICAL_QUEUE_IGNORED UINT_MAX
	extern std::unique_ptr<uint32_t[]> std_queueFamilyIndices;
	extern std::unique_ptr<VkQueue[]> std_queues;
	extern std::unique_ptr<VkQueueFlags[]> std_queueTypes;
	extern std::vector<bool> std_presentationAvailablePerQueue;
	extern unsigned uLogicalQueueCount;
	VkQueue get_present_queue(unsigned u8PreferredQueueIndex);

	class VulkanQueueCollection final {
		public:
			std::unique_ptr<uint32_t[]> std_queueFamilyIndices;
			std::unique_ptr<unsigned[]> std_logicalQueueIndices;
			VkSharingMode vk_eSharingMode;
			unsigned uQueueCount;

			VulkanQueueCollection() = default;

			VulkanQueueCollection(VulkanQueueCollection&&) = default;
			VulkanQueueCollection& operator =(VulkanQueueCollection&&) = default;

			VulkanQueueCollection(VulkanQueueCollection&) = delete;
			VulkanQueueCollection& operator =(VulkanQueueCollection&) = delete;
	};
	
	struct VulkanTask_Queues final {
		const unsigned *pauLogicalQueueIndices;
		const VkQueueFlagBits *vk_paeQueueTypes;
		const unsigned *pauStrictSeparationIds;
		unsigned uFunctionsCount;
	};

	class VulkanTask final {
		private:
			std::shared_ptr<unsigned[]> std_queueIndexPerCommandPool;
			std::unique_ptr<VkCommandPool[]> std_commandPools;
			std::shared_ptr<unsigned[]> std_commandPoolIndexPerCommandBuffer;
			std::unique_ptr<VkCommandBuffer[]> std_commandBuffers;
			VkSemaphore vk_hInternalSemaphore;
			unsigned uFunctionsCount;
			unsigned uCommandPoolCount;
			unsigned uLogicalPresentQueueIndex;
			bool bTransient;

		public:
			VulkanTask();
			VulkanTask(unsigned uFunctionsCount, const VkQueueFlagBits *vk_paeQueueTypePerFunctionRequiredInOrder, bool bIndividualResets, bool bIncludePresentation, bool bTransient);
			VulkanTask(unsigned uFunctionsCount, const unsigned *pau8LogicalQueueIndexPerFunctionRequiredInOrder, bool bIndividualResets, bool bIncludePresentation, bool bTransient);
			VulkanTask(const VulkanTask_Queues &rQueues, bool bIndividualResets, bool bIncludePresentation, bool bTransient);
			VulkanTask(const VulkanTask &rCopy, bool bIndividualResets, bool bTransient);
			VulkanTask(VulkanTask&) = delete;
			explicit VulkanTask(VulkanTask &&rrTask);
			~VulkanTask();
			bool init(unsigned uFunctionsCount, const VkQueueFlagBits *vk_paeQueueTypePerFunctionRequiredInOrder, bool bIndividualResets, bool bIncludePresentation, bool bTransient);
			bool init(unsigned uFunctionsCount, const unsigned *pau8LogicalQueueIndexPerFunctionRequiredInOrder, bool bIndividualResets, bool bIncludePresentation, bool bTransient);
			bool init(const VulkanTask_Queues &rQueues, bool bIndividualResets, bool bIncludePresentation, bool bTransient);
			bool init(const VulkanTask &rCopy, bool bIndividualResets, bool bTransient);
			void destroy();
			bool record(
					unsigned uFunctionIndex,
					VkCommandBufferUsageFlags vk_eUsageFlags,
					std::function<void (VkCommandBuffer vk_hCommandBuffer, unsigned u8PreviousLogicalQueue, unsigned u8CurrentLogicalQueue, unsigned u8NextLogicalQueue)> recorderFunction) const;
			bool submit(
					uint32_t u32SemaphoresToWaitForCount,
					const VkSemaphoreSubmitInfo *vk_paSemaphoresToWaitFor,
					const VkPipelineStageFlags2 *vk_paeInternSemaphoreWaits,
					uint32_t u32SemaphoresToSignal,
					const VkSemaphoreSubmitInfo *vk_paSemaphoresToSignal,
					VkFence vk_hFenceToSignal) const;
			void reset_all(VkCommandPoolResetFlags vk_eResetFlags = 0) const;
			VkCommandPool command_pool_of_function(unsigned uFunctionIndex) const;
			unsigned function_count() const;
			unsigned logical_queue_index_for_function(unsigned uFunctionIndex) const;
			unsigned logical_queue_index_for_presentation() const;
			VulkanQueueCollection queues_of_functions(const unsigned *pauFunctionIndices, unsigned uFunctionIndexCount, bool bIncludePresentation) const;
			bool valid() const;
	};

	// Memory
	class VulkanMemory;
	using VulkanStorageObject = std::variant<VkBuffer, VkImage>;
	void fetch_vulkan_memory_requirements(VulkanStorageObject vulkanStorageObject, VkMemoryRequirements2 &vk_rMemoryRequirements);
	bool bind_vulkan_memory(VulkanStorageObject vulkanStorageObject, VkDeviceMemory vk_hMemory, VkDeviceSize vk_memoryOffset = 0);
	struct SharedVulkanMemoryInfo final {
		VulkanStorageObject vulkanStorageObject;
		unsigned uRegionIndex;
	};
	struct VulkanMemoryAllocationInfo final {
		size_t indexToMemory;
		VkDeviceSize vk_memoryOffset;
	};
	VkResult alloc_shared_vulkan_memory(
			uint32_t u32SharedMemoryInfoCount,
			const SharedVulkanMemoryInfo *paSharedMemoryInfos,
			VkMemoryPropertyFlags vk_mMemoryProperties,
			size_t &rAllocatedMemoryCount,
			std::unique_ptr<VulkanMemory[]> &rAllocatedMemory,
			VulkanMemoryAllocationInfo *paAllocationResults = nullptr,
			bool *pbVulkanStorageObjectsUnbound = nullptr);
	std::optional<unsigned> find_vulkan_memory_type(VkMemoryPropertyFlags vk_mProperties, uint32_t m32MemoryTypeBits);
	bool do_memory_properties_exist(VkMemoryPropertyFlags vk_mProperties);
	bool is_staging_before_gpu_use_necessary();
	uint32_t get_remaining_vulkan_allocations();
	class VulkanMemory final {
		private:
			VkDeviceMemory vk_hMemory;
			VkDeviceSize vk_size;
			unsigned uMemoryType;
			bool bCoherent,
				bMapped;

		public:
			VulkanMemory();
			VulkanMemory(VkDeviceSize vk_size, VkMemoryPropertyFlags vk_mProperties, uint32_t m32DesiredMemoryTypes);
			VulkanMemory(VkDeviceSize vk_size, unsigned uMemoryTypeIndex);
			VulkanMemory(VulkanMemory &rMemory) = delete;
			explicit VulkanMemory(VulkanMemory &&rrMemory);
			~VulkanMemory();
			
			VkResult alloc(VkDeviceSize vk_size, VkMemoryPropertyFlags vk_mProperties, uint32_t m32DesiredMemoryTypes);
			VkResult alloc(VkDeviceSize vk_size, unsigned uMemoryTypeIndex);
			VkResult alloc_for_buffer(VkBuffer vk_hBuffer, VkMemoryPropertyFlags vk_mProperties);
			VkResult alloc_for_image(VkImage vk_hImage, VkMemoryPropertyFlags vk_mProperties);
			void free();
			bool map(VkMemoryMapFlags vk_eFlags, VkDeviceSize vk_offset, VkDeviceSize vk_size, void **ppData);
			void unmap();
			bool flush_mapped_memory(VkDeviceSize vk_offset = 0, VkDeviceSize vk_size = VK_WHOLE_SIZE);
			bool invalidate_mapped_memory(VkDeviceSize vk_offset = 0, VkDeviceSize vk_size = VK_WHOLE_SIZE);

			bool valid() const;
			VkDeviceMemory get() const;
			VkDeviceSize size() const;
			unsigned type_index() const;
			bool cpu_coherent() const;
			bool mapped() const;

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
