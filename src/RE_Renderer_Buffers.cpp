#include "RE_Renderer_Buffers_Internal.hpp"
#include "RE_ListBatch_GameObject.hpp"
#include "RE_List_GameObject.hpp"

namespace RE {

	size_t gameObjectCounterBuffer = 0, gameObjectsToRenderCount;
	
	bool create_renderer_buffers() {
		PRINT_DEBUG("Creating buffers in renderer");
		gameObjectCounterBuffer = std::clamp<size_t>(gameObjectBatchList.size(), 3, std::numeric_limits<size_t>::max()) * MAXIMUM_GAME_OBJECTS_PER_BATCH;
		if (create_processing_buffers()) {
			if (create_uniform_buffers()) {
				if (create_object_buffers())
					return true;
				destroy_uniform_buffers();
			}
			destroy_processing_buffers();
		}
		return false;
	}

	void destroy_renderer_buffers() {
		destroy_object_buffers();
		destroy_uniform_buffers();
		destroy_processing_buffers();
	}

}
