#include "RE_Renderer_Buffers_Internal.hpp"
#include "RE_ListBatch_GameObject.hpp"
#include "RE_List_GameObject.hpp"

namespace RE {
	
	bool create_renderer_buffers() {
		PRINT_DEBUG("Creating buffers for the renderer");
		constexpr uint32_t au32FunctionIndices[] = {
			RENDER_TASK_SUBINDEX_PROCESSING,
			RENDER_TASK_SUBINDEX_RENDERING
		};
		const VulkanQueueCollection queues = renderTasks[0].queues_of_functions(au32FunctionIndices, sizeof(au32FunctionIndices) / sizeof(au32FunctionIndices[0]));
		if (create_game_object_buffers(queues)) {
			if (create_sortable_depth_buffers(queues)) {
				if (create_camera_buffers()) {
					if (create_raw_game_object_buffers())
						return true;
					destroy_camera_buffers();
				}
				destroy_sortable_depth_buffers();
			}
			destroy_game_object_buffers();
		}
		return false;
	}

	void destroy_renderer_buffers() {
		destroy_raw_game_object_buffers();
		destroy_camera_buffers();
		destroy_sortable_depth_buffers();
		destroy_game_object_buffers();
	}

}
