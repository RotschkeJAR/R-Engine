#include "RE_Renderer_WindowFrame_Internal.hpp"

namespace RE {

	bool init_window_frame() {
		if (create_window_frame_buffers()) {
			if (create_window_frame_images()) {
				if (alloc_window_frame_memory()) {
					if (create_window_frame_image_views()) {
						if (create_window_frame_descriptor_sets()) {
							if (create_window_frame_graphics_pipelines()) {
								return true;
								destroy_window_frame_graphics_pipelines();
							}
							destroy_window_frame_descriptor_sets();
						}
						destroy_window_frame_image_views();
					}
					free_window_frame_resources();
				}
				destroy_window_frame_images();
			}
			destroy_window_frame_buffers();
		}
		return false;
	}

	void destroy_window_frame() {
		destroy_window_frame_graphics_pipelines();
		destroy_window_frame_descriptor_sets();
		destroy_window_frame_image_views();
		free_window_frame_memory();
		destroy_window_frame_images();
		destroy_window_frame_buffers();
	}

}
