#ifndef __RE_RENDERER_BUFFERS_INTERNAL_H__
#define __RE_RENDERER_BUFFERS_INTERNAL_H__

#include "RE_Renderer_Buffers.hpp"

namespace RE {

	bool create_staging_game_objects_buffer();
	void destroy_staging_game_objects_buffer();

	bool create_game_objects_buffers();
	void destroy_game_objects_buffers();

	bool create_sortable_depth_buffers(const VulkanQueueCollection &rQueues);
	void destroy_sortable_depth_buffers();

	bool create_game_objects_model_matrix_buffers(const VulkanQueueCollection &rQueues);
	void destroy_game_objects_model_matrix_buffers();
	
	bool create_camera_buffers(const VulkanQueueCollection &rQueues);
	void destroy_camera_buffers();

	bool create_cursor_buffers();
	void destroy_cursor_buffers();

	bool alloc_memory_for_renderer_buffers();
	void free_memory_for_renderer_buffers();

}

#endif /* __RE_RENDERER_BUFFERS_INTERNAL_H__ */
