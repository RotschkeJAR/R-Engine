#ifndef __RE_RENDERER_DESCRIPTOR_SETS_H__
#define __RE_RENDERER_DESCRIPTOR_SETS_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {

	extern VkDescriptorSet vk_hTextureDescSet, 
		vk_ahRenderContentDescSets[RE_VK_FRAMES_IN_FLIGHT], 
		vk_ahRawGameObjectBufferDescSets[RE_VK_FRAMES_IN_FLIGHT];
	
	bool create_descriptor_sets();
	void destroy_descriptor_sets();

	void calculate_camera_matrices();

}

#endif /* __RE_RENDERER_DESCRIPTOR_SETS_H__ */
