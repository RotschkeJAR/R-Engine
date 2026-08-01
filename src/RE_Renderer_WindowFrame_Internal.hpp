#ifndef __RE_RENDERER_WINDOW_FRAME_INTERNAL_H__
#define __RE_RENDERER_WINDOW_FRAME_INTERNAL_H__ 1

#include "RE_Renderer_WindowFrame.hpp"
#include "RE_Renderer_WindowFrame_Images.hpp"
#include "RE_Renderer_WindowFrame_Buffers.hpp"
#include "RE_Renderer_WindowFrame_DescriptorSets.hpp"
#include "RE_Renderer_WindowFrame_GraphicsPipelines.hpp"
#include "RE_Renderer_WindowFrame_Memory.hpp"

namespace RE {
	
#define WINDOW_FRAME_RENDER_MODE_SHADOWS  0
#define WINDOW_FRAME_RENDER_MODE_BAR      1
#define WINDOW_FRAME_RENDER_MODE_BUTTONS  2
#define WINDOW_FRAME_RENDER_MODE_TITLE    3

}

#endif /* __RE_RENDERER_WINDOW_FRAME_INTERNAL_H__ */
