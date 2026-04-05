#include "RE_GPU_Internal.hpp"
#include "RE_Renderer.hpp"
#include "RE_Window.hpp"
#include "RE_Main.hpp"

namespace RE {

	bool init_logical_gpu() {
		if (init_vulkan_instance()) {
			if (create_vulkan_surface()) {
				if (alloc_physical_vulkan_device_list()) {
					fetch_gpu_constrains();
					fetch_vulkan_surface_infos();
					select_best_vulkan_surface_format();
					find_suitable_depth_stencil_formats();
					if (init_logical_vulkan_device()) {
						if (setup_logical_device_queues())
							return true;
						destroy_logical_vulkan_device();
					}
					free_physical_vulkan_device_list();
				}
				destroy_vulkan_surface();
			}
			destroy_vulkan_instance();
		}
		return false;
	}

	void destroy_logical_gpu() {
		destroy_logical_device_queues();
		destroy_logical_vulkan_device();
		free_gpu_memory_info();
		free_physical_vulkan_device_list();
		destroy_vulkan_surface();
		destroy_vulkan_instance();
	}

}
