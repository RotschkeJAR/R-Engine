#ifndef __RE_RENDERER_WINDOW_FRAME_DESCRIPTOR_SETS_H__
#define __RE_RENDERER_WINDOW_FRAME_DESCRIPTOR_SETS_H__

#include "RE_Renderer_WindowFrame_Internal.hpp"

namespace RE {

	extern VkDescriptorSetLayout vk_hWindowFrameDescSetLayout;
	extern VkDescriptorSet vk_hWindowFrameDescSet;

	bool create_window_frame_descriptor_sets();
	void destroy_window_frame_descriptor_sets();

}

#endif /* __RE_RENDERER_WINDOW_FRAME_DESCRIPTOR_SETS_H__ */
