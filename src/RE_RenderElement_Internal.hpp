#ifndef __RE_RENDER_ELEMENT_INTERNAL_H__
#define __RE_RENDER_ELEMENT_INTERNAL_H__

#include "RE_RenderElement.hpp"

namespace RE {
	
	// Game Objects
	bool init_render_element_rectangle(Vulkan_Buffer &rStagingRectBuffer);
	void destroy_render_element_rectangle();

}

#endif /* __RE_RENDER_ELEMENT_INTERNAL_H__ */
