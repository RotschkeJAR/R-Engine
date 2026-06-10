#ifndef __RE_RENDERER_DESCRIPTOR_SET_LAYOUTS_H__
#define __RE_RENDERER_DESCRIPTOR_SET_LAYOUTS_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {

#define RE_VK_GAME_OBJECTS_DESC_SET_BINDING_COUNT 2
#define RE_VK_GAME_OBJECTS_DESC_SET_INSTANCE_BINDING_INDEX 0
#define RE_VK_GAME_OBJECTS_DESC_SET_MODEL_MATRIX_BINDING_INDEX 1

#define RE_VK_SPRITE_DESC_SET_BINDING_COUNT 2
#define RE_VK_SPRITE_DESC_SET_SPRITE_LAYOUT_BINDING_INDEX 0
#define RE_VK_SPRITE_DESC_SET_TEXTURE_BINDING_INDEX 1

	extern VkDescriptorSetLayout vk_hGameObjectsDescSetLayout,
		vk_hSortableDepthDescSetLayout,
		vk_hCameraDescSetLayout,
		vk_hSpriteDescSetLayout;
#ifdef RE_OS_LINUX
	extern VkDescriptorSetLayout vk_hWindowFrameDescSetLayout;
#endif

	bool create_descriptor_set_layouts();
	void destroy_descriptor_set_layouts();

}

#endif /* __RE_RENDERER_DESCRIPTOR_SET_LAYOUTS_H__ */
