#ifndef __RE_RENDERER_GAME_OBJECT_H__
#define __RE_RENDERER_GAME_OBJECT_H__

#include "RE_Render System.hpp"

namespace RE {

	extern VkCommandBuffer vk_ahOpaqueGameObjectSecondaryCommandBuffers[RE_VK_FRAMES_IN_FLIGHT];
	
	bool init_opaque_gameobject_renderer();
	void destroy_opaque_gameobject_renderer();
	bool render_opaque_gameobjects();

}

#endif /* __RE_RENDERER_GAME_OBJECT_H__ */
