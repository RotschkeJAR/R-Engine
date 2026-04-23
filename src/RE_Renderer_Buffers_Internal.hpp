#ifndef __RE_RENDERER_BUFFERS_INTERNAL_H__
#define __RE_RENDERER_BUFFERS_INTERNAL_H__

#include "RE_Renderer_Buffers.hpp"

namespace RE {

	bool create_raw_game_object_buffers();
	void destroy_raw_game_object_buffers();

	bool create_sortable_depth_buffers(const VulkanQueueCollection &rQueues);
	void destroy_sortable_depth_buffers();

	bool create_game_object_buffers(const VulkanQueueCollection &rQueues);
	void destroy_game_object_buffers();
	
	bool create_camera_buffers();
	void destroy_camera_buffers();

	bool alloc_memories_for_buffers_renderer();
	void free_memories_for_buffers_renderer();

}

#endif /* __RE_RENDERER_BUFFERS_INTERNAL_H__ */
