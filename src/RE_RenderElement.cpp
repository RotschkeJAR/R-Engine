#include "RE_RenderElement_Internal.hpp"

namespace RE {
	
	bool init_render_elements(Vulkan_Buffer &rStagingRectBuffer, VulkanTask &rRectBufferCreateTask, Vulkan_Fence &rRectBufferTransferFence) {
		if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(init_render_element_rectangle(rStagingRectBuffer, rRectBufferCreateTask, rRectBufferTransferFence), bool))
			return true;
		return false;
	}

	void destroy_render_elements() {
		PUSH_TO_CALLSTACKTRACE(destroy_render_element_rectangle());
	}

}
