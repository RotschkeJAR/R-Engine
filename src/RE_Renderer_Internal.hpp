#ifndef __RE_RENDERER_INTERNAL_H__
#define __RE_RENDERER_INTERNAL_H__

#include "RE_Renderer.hpp"
#include "RE_Render System.hpp"
#include "RE_Texture.hpp"

namespace RE {

	// Index buffer
	extern VkBuffer vk_hRectIndexBuffer;

	// MSAA
	extern VkSampleCountFlagBits vk_eMsaaCount;

}

#endif /* __RE_RENDERER_INTERNAL_H__ */
