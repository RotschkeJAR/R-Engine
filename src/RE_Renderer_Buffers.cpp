#include "RE_Renderer_Buffers_Internal.hpp"
#include "RE_List_GameObject.hpp"

namespace RE {

	bool create_renderer_buffers() {
		PRINT_DEBUG("Creating buffers for the renderer");
		constexpr uint32_t au32FunctionIndices[] = {
			RENDER_TASK_SUBINDEX_PROCESSING,
			RENDER_TASK_SUBINDEX_RENDERING
		};
		const VulkanQueueCollection queues = renderTasks[0].queues_of_functions(au32FunctionIndices, sizeof(au32FunctionIndices) / sizeof(au32FunctionIndices[0]));
		if (create_game_objects_model_matrix_buffers(queues)) {
			if (create_sortable_depth_buffers(queues)) {
				if (create_camera_buffers(queues)) {
					if (create_game_objects_buffers()) {
						if (create_staging_game_objects_buffer()) {
							if (alloc_memory_for_renderer_buffers())
								return true;
							destroy_staging_game_objects_buffer();
						}
						destroy_game_objects_buffers();
					}
					destroy_camera_buffers();
				}
				destroy_sortable_depth_buffers();
			}
			destroy_game_objects_model_matrix_buffers();
		}
		return false;
	}

	void destroy_renderer_buffers() {
		PRINT_DEBUG("Destroying buffers and freeing memory used in the renderer");
		free_memory_for_renderer_buffers();
		destroy_staging_game_objects_buffer();
		destroy_game_objects_buffers();
		destroy_camera_buffers();
		destroy_sortable_depth_buffers();
		destroy_game_objects_model_matrix_buffers();
	}

}
