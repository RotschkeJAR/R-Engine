#include "RE_RenderElement_Internal.hpp"

namespace RE {
	
	bool init_render_elements(Vulkan_Buffer &rStagingRectBuffer, VulkanTask *pTransferTask, Vulkan_Fence &rTransferFence) {
		PRINT_DEBUG("Initializing render elements");
		return init_render_element_rectangle(rStagingRectBuffer, pTransferTask, rTransferFence);
	}

	void destroy_render_elements() {
		PRINT_DEBUG("Destroying render elements");
		destroy_render_element_rectangle();
	}

}
