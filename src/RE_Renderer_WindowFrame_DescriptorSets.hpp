#ifndef __RE_RENDERER_WINDOW_FRAME_DESCRIPTOR_SETS_H__
#define __RE_RENDERER_WINDOW_FRAME_DESCRIPTOR_SETS_H__ 1

#include "RE_Renderer_WindowFrame_Internal.hpp"

namespace RE {

#ifdef RENDERER_INCLUDE_WINDOW_FRAME

	extern VkDescriptorSetLayout vk_hWindowFrameDescSetLayout;
	extern VkDescriptorSet vk_hWindowFrameDescSet;

	bool create_window_frame_descriptor_sets();
	void destroy_window_frame_descriptor_sets();

#endif

}

#endif /* __RE_RENDERER_WINDOW_FRAME_DESCRIPTOR_SETS_H__ */
