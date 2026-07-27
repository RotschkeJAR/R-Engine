#ifndef __RE_RENDERER_WINDOW_FRAME_MEMORY_H__
#define __RE_RENDERER_WINDOW_FRAME_MEMORY_H__ 1

#include "RE_Renderer_WindowFrame_Internal.hpp"

namespace RE {

#ifdef RENDERER_INCLUDE_WINDOW_FRAME

	bool alloc_window_frame_memory();
	void free_window_frame_memory();

#endif

}

#endif /* __RE_RENDERER_WINDOW_FRAME_MEMORY_H__ */
