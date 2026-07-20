#ifndef __RE_RENDERER_DYNAMIC_RENDERING_H__
#define __RE_RENDERER_DYNAMIC_RENDERING_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {
	
	void record_cmd_begin_dynamic_rendering(VkCommandBuffer vk_hCommandBuffer);
	void record_cmd_end_dynamic_rendering(VkCommandBuffer vk_hCommandBuffer);

}

#endif /* __RE_RENDERER_DYNAMIC_RENDERING_H__ */
