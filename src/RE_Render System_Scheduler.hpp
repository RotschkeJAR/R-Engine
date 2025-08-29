#ifndef __RE_RENDER_SYSTEM_SCHEDULER_H__
#define __RE_RENDER_SYSTEM_SCHEDULER_H__

#include "RE_Vulkan_Device.hpp"
#include "RE_Internal Header.hpp"

#include <queue>

namespace RE {

	void does_gpu_have_necessary_queues(VkPhysicalDevice vk_hPhysicalDevice, std::queue<std::string> &rMissingFeatures);
	[[nodiscard]]
	int32_t rate_gpu_queues(VkPhysicalDevice vk_hPhysicalDevice);
	void create_queue_create_infos(const float &fPriority, std::vector<VkDeviceQueueCreateInfo> &vk_rpaLogicalQueueCreateInfos);
	bool setup_logical_device_interfaces();
	void destroy_logical_device_interfaces();

}

#endif /* __RE_RENDER_SYSTEM_SCHEDULER_H__ */
