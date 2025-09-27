#include "RE_RenderElement_Internal.hpp"

namespace RE {
	
	bool init_render_elements(Vulkan_Buffer &rStagingRectBuffer, VulkanTask &rRectBufferCreateTask, Vulkan_Fence &rRectBufferTransferFence) {
		return init_render_element_rectangle(rStagingRectBuffer, rRectBufferCreateTask, rRectBufferTransferFence);
	}

	void destroy_render_elements() {
		destroy_render_element_rectangle();
	}

}
