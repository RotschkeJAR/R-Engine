#ifndef __RE_RENDERER_INTERNAL_H__
#define __RE_RENDERER_INTERNAL_H__ 1

#include "RE_Renderer.hpp"
#include "RE_Vulkan_Wrappers.hpp"
#include "RE_Settings.hpp"

#include "RE_Renderer_ShaderStructs.hpp"
#include "RE_Renderer_DescriptorSets.hpp"
#include "RE_Renderer_DescriptorSetPools.hpp"
#include "RE_Renderer_DescriptorSetLayouts.hpp"
#include "RE_Renderer_Pipelines.hpp"
#include "RE_Renderer_Buffers.hpp"
#include "RE_Renderer_Camera.hpp"
#include "RE_Renderer_Images.hpp"
#include "RE_Renderer_Texture.hpp"
#include "RE_Renderer_SpriteLayout.hpp"
#include "RE_Renderer_Mesh.hpp"
#include "RE_Renderer_Swapchain.hpp"
#include "RE_Renderer_Presentation.hpp"
#include "RE_Renderer_RenderTask.hpp"
#include "RE_Renderer_RenderProcedure.hpp"
#include "RE_Renderer_WindowFrame.hpp"
#include "RE_Renderer_Framebuffers.hpp"
#include "RE_Renderer_RenderPass.hpp"

namespace RE {

#define get_max_camera_count()        5
#define get_max_game_object_count()   1024

	bool swapchain_created_renderer();
	void swapchain_destroyed_renderer();

	extern Color backgroundClearColor;
	extern unsigned uCurrentFrameInFlightIndex;

}

#endif /* __RE_RENDERER_INTERNAL_H__ */
