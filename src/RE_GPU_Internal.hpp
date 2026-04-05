#ifndef __RE_GPU_INTERNAL_H__
#define __RE_GPU_INTERNAL_H__

#include "RE_GPU.hpp"
#include "RE_Renderer.hpp"

namespace RE {

	// Vulkan instance
	bool init_vulkan_instance();
	void destroy_vulkan_instance();

	// Surface
	void destroy_vulkan_surface();
	void fetch_vulkan_surface_infos();
	void select_best_vulkan_surface_format();

	// Physical Device
	bool alloc_physical_vulkan_device_list();
	void free_physical_vulkan_device_list();

	// Physical Device Suitability
	bool is_physical_vulkan_device_suitable(VkPhysicalDevice vk_hPhysicalDevice, int32_t *pi32Score);

	// Vulkan Device
	bool init_logical_vulkan_device();
	void destroy_logical_vulkan_device();
	PFN_vkVoidFunction load_vulkan_func_with_device(const char *pacFuncName);

	// Synchronization2
	bool load_synchronization_2_funcs();
	void unload_synchronization_2_funcs();
	
	// Scheduler
	bool setup_logical_device_queues();
	void destroy_logical_device_queues();

	// Memory
	extern std::unique_ptr<std::tuple<VkMemoryHeap, VkDeviceSize>[]> vulkanMemoryHeaps;
	extern std::unique_ptr<VkMemoryType[]> vulkanMemoryTypes;
	extern uint32_t u32VulkanMemoryAllocCount;
	extern uint8_t u8MemoryHeapCount, u8MemoryTypeCount;
	void alloc_gpu_memory_info();
	void free_gpu_memory_info();

	// Constrains
	void fetch_gpu_constrains();

}

#endif /* __RE_GPU_INTERNAL_H__ */
