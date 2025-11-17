#ifndef __RE_RENDER_PIPELINES_H__
#define __RE_RENDER_PIPELINES_H__

#include "RE_Renderer.hpp"

namespace RE {

	struct GameObjectInstanceData final {
		float a16fModelMatrix[16];
		float a4fColor[4];
		uint32_t u32TextureId;
	};

#define RE_VK_RENDERPIPELINE_COUNT 2
#define RE_VK_OPAQUE_RENDERPIPELINE_INDEX 0
#define RE_VK_TRANSPARENT_RENDERPIPELINE_INDEX 1
	
	extern VkPipeline vk_ahGameObjectPipelines[RE_VK_RENDERPIPELINE_COUNT];

	bool create_render_pipelines();
	bool recreate_render_pipelines();
	void destroy_render_pipelines();

}

#endif /* __RE_RENDER_PIPELINES_H__ */
