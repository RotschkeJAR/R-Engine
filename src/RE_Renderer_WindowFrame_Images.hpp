#ifndef __RE_RENDERER_WINDOW_FRAME_IMAGES_H__
#define __RE_RENDERER_WINDOW_FRAME_IMAGES_H__

#include "RE_Renderer_WindowFrame_Internal.hpp"

namespace RE {

#ifdef RENDERER_INCLUDE_WINDOW_FRAME

	extern VkImage vk_hWindowButtonImage;
	extern VkImageView vk_hWindowButtonImageView;

	bool create_window_frame_images();
	bool create_window_frame_image_views();
	void destroy_window_frame_images();
	void destroy_window_frame_image_views();

#endif

}

#endif /* __RE_RENDERER_WINDOW_FRAME_IMAGES_H__ */
