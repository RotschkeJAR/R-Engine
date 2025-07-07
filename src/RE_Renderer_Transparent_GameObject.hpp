#ifndef __RE_RENDERER_TRANSPARENT_GAME_OBJECT_H__
#define __RE_RENDERER_TRANSPARENT_GAME_OBJECT_H__

#include "RE_Renderer.hpp"

#include <queue>

namespace RE {

	extern std::queue<GameObject*> queuedTransparentGameObjects;
	
	extern VkCommandBuffer vk_ahTransparentGameObjectSecondaryCommandBuffers[RE_VK_FRAMES_IN_FLIGHT];

	bool init_transparent_game_object_renderer();
	void destroy_transparent_game_object_renderer();
	bool render_transparent_game_object();

}

#endif /* __RE_RENDERER_TRANSPARENT_GAME_OBJECT_H__ */
