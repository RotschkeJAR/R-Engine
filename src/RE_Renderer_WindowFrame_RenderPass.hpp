#ifndef __RE_RENDERER_WINDOW_FRAME_RENDER_PASS_H__
#define __RE_RENDERER_WINDOW_FRAME_RENDER_PASS_H__ 1

#include "RE_Renderer_WindowFrame_Internal.hpp"

namespace RE {

#ifdef RENDERER_INCLUDE_WINDOW_FRAME

	bool create_window_frame_render_pass();
	void destroy_window_frame_render_pass();

#endif

}

#endif /* __RE_RENDERER_WINDOW_FRAME_RENDER_PASS_H__ */
