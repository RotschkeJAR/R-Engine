#ifndef __RE_RENDERER_DESCRIPTOR_SETS_INTERNAL_H__
#define __RE_RENDERER_DESCRIPTOR_SETS_INTERNAL_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {

	extern VkDescriptorPool vk_hPermanentDescPool;

	// Camera
	bool create_camera_descriptor_sets();
	void destroy_camera_descriptor_sets();

	// Computing
	bool create_computing_descriptor_sets();
	void destroy_computing_descriptor_sets();
	
	// Textures
	bool create_texture_descriptor_sets();
	void destroy_texture_descriptor_sets();

}

#endif /* __RE_RENDERER_DESCRIPTOR_SETS_INTERNAL_H__ */
