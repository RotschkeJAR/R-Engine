#ifndef __RE_RENDERER_RENDER_PROCEDURE_INTERNAL_H__
#define __RE_RENDERER_RENDER_PROCEDURE_INTERNAL_H__ 1

#include "RE_Renderer_RenderProcedure.hpp"

namespace RE {

#define RENDERING_TIMEOUT_SEC   std::chrono::seconds(5)

	void copy_staging_game_objects(VkCommandBuffer vk_hCommandBuffer);

	void render_pass(VkCommandBuffer vk_hCommandBuffer);
	void swapchain_render_pass(VkCommandBuffer vk_hCommandBuffer);
	
	void process_game_objects(VkCommandBuffer vk_hCommandBuffer);
	void sort_game_objects(VkCommandBuffer vk_hCommandBuffer);

	void blit_render_image(VkCommandBuffer vk_hCommandBuffer);

}

#endif /* __RE_RENDERER_RENDER_PROCEDURE_INTERNAL_H__ */
