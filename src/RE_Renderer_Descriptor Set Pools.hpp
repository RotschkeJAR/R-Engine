#ifndef __RE_RENDERER_DESCRIPTOR_SET_POOLS_H__
#define __RE_RENDERER_DESCRIPTOR_SET_POOLS_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {
	
	extern VkDescriptorPool vk_hPersistentDescPool;

	bool create_descriptor_set_pools();
	void destroy_descriptor_set_pools();

}

#endif /* __RE_RENDERER_DESCRIPTOR_SET_POOLS_H__ */
