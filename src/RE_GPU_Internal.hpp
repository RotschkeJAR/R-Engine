#ifndef __RE_GPU_INTERNAL_H__
#define __RE_GPU_INTERNAL_H__

#include "RE_GPU.hpp"
#include "RE_Renderer.hpp"

namespace RE {

	extern bool bVsyncEnabled;

	// Surface
	extern VkSurfaceKHR vk_hSurface;
	extern VkSurfaceCapabilitiesKHR vk_surfaceCapabilities;
	extern std::unique_ptr<VkSurfaceFormatKHR[]> vk_paSurfaceFormatsAvailable;
	extern uint32_t u32SurfaceFormatsAvailableCount;
	extern uint32_t u32IndexToSelectedSurfaceFormat;

	// Physical Device
	bool alloc_physical_vulkan_device_list();
	void free_physical_vulkan_device_list();
	void select_best_physical_vulkan_device();
	void select_physical_vulkan_device(VkPhysicalDevice vk_hPhysicalDevice);
	
	// Scheduler
	void does_gpu_have_necessary_queues(VkPhysicalDevice vk_hPhysicalDevice, std::queue<std::string> &rMissingFeatures);
	int32_t rate_gpu_queues(VkPhysicalDevice vk_hPhysicalDevice);
	bool setup_logical_device_queues();
	void destroy_logical_device_queues();

	// Swapchain
	extern VkPresentModeKHR vk_ePresentModeVsync, vk_ePresentModeNoVsync;
	bool recreate_swapchain();

	// Memory
	extern std::vector<std::tuple<VkMemoryHeap, VkDeviceSize>> vulkanMemoryHeaps;
	extern std::vector<VkMemoryType> vulkanMemoryTypes;
	extern uint32_t u32VulkanMemoryAllocCount;
	bool does_gpu_support_memory(VkPhysicalDevice vk_hPhysicalDevice, const VkPhysicalDeviceLimits &vk_rPhysicalDeviceLimits, std::queue<std::string> &rMissingFeatures);
	int32_t rate_gpu_memory_capacity(VkPhysicalDevice vk_hPhysicalDevice, const VkPhysicalDeviceProperties &vk_rPhysicalDeviceProperties);
	void fetch_gpu_memory_info();

}

#endif /* __RE_GPU_INTERNAL_H__ */
