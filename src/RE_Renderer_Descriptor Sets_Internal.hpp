#ifndef __RE_RENDERER_DESCRIPTOR_SETS_INTERNAL_H__
#define __RE_RENDERER_DESCRIPTOR_SETS_INTERNAL_H__

#include "RE_Renderer_Descriptor Sets.hpp"

namespace RE {

	extern VkDescriptorPool vk_hPermanentDescPool;

	// Layouts
	bool create_descriptor_set_layouts();
	void destroy_descriptor_set_layouts();

	// Camera
	bool create_camera_descriptor_sets();
	void destroy_camera_descriptor_sets();

	// Processing
	bool create_processing_descriptor_sets();
	void destroy_processing_descriptor_sets();
	
	// Textures
	bool create_texture_descriptor_sets();
	void destroy_texture_descriptor_sets();

}

#endif /* __RE_RENDERER_DESCRIPTOR_SETS_INTERNAL_H__ */
