#include "RE_Renderer_WindowFrame_Internal.hpp"

namespace RE {

#ifdef RENDERER_INCLUDE_WINDOW_FRAME

	bool init_window_frame() {
		if (create_window_frame_buffers()) {
			if (create_window_frame_images()) {
				if (alloc_window_frame_memory()) {
					if (create_window_frame_image_views()) {
						if (create_window_frame_descriptor_sets()) {
							if (create_window_frame_graphics_pipelines()) {
								PRINT_DEBUG("Writing parameters for window frame rendering");
								pIndirectDrawWindowTitle->vertexCount = 4;
								pIndirectDrawWindowTitle->firstVertex = 0;
								pIndirectDrawWindowTitle->firstInstance = 0;
								for (pIndirectDrawWindowTitle->instanceCount = 0; pIndirectDrawWindowTitle->instanceCount < 256; pIndirectDrawWindowTitle->instanceCount++) {
									const uint32_t u32CharCode = static_cast<uint32_t>(pacWindowTitle[pIndirectDrawWindowTitle->instanceCount]);
									pWindowFrameUniformData->au32TitleChars[pIndirectDrawWindowTitle->instanceCount] = u32CharCode;
									if (u32CharCode == 0)
										break;
								}
								return true;
							}
							destroy_window_frame_descriptor_sets();
						}
						destroy_window_frame_image_views();
					}
					free_window_frame_memory();
				}
				destroy_window_frame_images();
			}
			destroy_window_frame_buffers();
		}
		return false;
	}

	void destroy_window_frame() {
		destroy_window_frame_graphics_pipelines();
		destroy_window_frame_descriptor_sets();
		destroy_window_frame_image_views();
		free_window_frame_memory();
		destroy_window_frame_images();
		destroy_window_frame_buffers();
	}

	void render_window_frame(VkCommandBuffer vk_hCommandBuffer) {
		if (should_render_window_frame_bar()) {
			vkCmdBindPipeline(vk_hCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hWindowFrameGraphicsPipeline);
			const VkDescriptorSet vk_ahDescSets[] = {
				vk_hWindowFrameDescSet,
				vk_hCharacterDescSet
			};
			vkCmdBindDescriptorSets(
					vk_hCommandBuffer,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					vk_hWindowFramePipelineLayout,
					0,
					sizeof(vk_ahDescSets) / sizeof(vk_ahDescSets[0]),
					vk_ahDescSets,
					0,
					nullptr);
			const VkViewport vk_viewport = {
				.x = 0.0f,
				.y = 0.0f,
				.width = static_cast<float>(vk_swapchainResolution.width),
				.height = static_cast<float>(vk_swapchainResolution.height),
				.minDepth = 0.0f,
				.maxDepth = 1.0f
			};
			vkCmdSetViewport(vk_hCommandBuffer, 0, 1, &vk_viewport);
			const VkRect2D vk_scissor = {
				.offset = {
					.x = 0,
					.y = 0
				},
				.extent = vk_swapchainResolution
			};
			vkCmdSetScissor(vk_hCommandBuffer, 0, 1, &vk_scissor);
			WindowShaderData windowPushConstants;
			windowPushConstants.a2u32Size[0] = vk_swapchainResolution.width;
			windowPushConstants.a2u32Size[1] = vk_swapchainResolution.height;
			if (should_render_window_frame_edges()) {
				windowPushConstants.u32WindowFrameToRender = WINDOW_FRAME_RENDER_MODE_SHADOWS;
				windowPushConstants.b32RenderEdges = SHR_TRUE;
				vkCmdPushConstants(
						vk_hCommandBuffer,
						vk_hWindowFramePipelineLayout,
						VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
						0,
						sizeof(WindowShaderData),
						static_cast<const void*>(&windowPushConstants));
				vkCmdDraw(vk_hCommandBuffer, 10, 1, 0, 0);
				windowPushConstants.u32WindowFrameToRender = WINDOW_FRAME_RENDER_MODE_BAR;
				vkCmdPushConstants(
						vk_hCommandBuffer,
						vk_hWindowFramePipelineLayout,
						VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
						offsetof(WindowShaderData, u32WindowFrameToRender),
						sizeof(WindowShaderData::u32WindowFrameToRender),
						static_cast<const void*>(&windowPushConstants));
				vkCmdDraw(vk_hCommandBuffer, 10, 1, 0, 0);
			} else {
				windowPushConstants.u32WindowFrameToRender = WINDOW_FRAME_RENDER_MODE_BAR;
				windowPushConstants.b32RenderEdges = SHR_FALSE;
				vkCmdPushConstants(
						vk_hCommandBuffer,
						vk_hWindowFramePipelineLayout,
						VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
						0,
						sizeof(WindowShaderData),
						static_cast<const void*>(&windowPushConstants));
				vkCmdDraw(vk_hCommandBuffer, 4, 1, 0, 0);
			}
			windowPushConstants.u32WindowFrameToRender = WINDOW_FRAME_RENDER_MODE_BUTTONS;
			vkCmdPushConstants(
					vk_hCommandBuffer,
					vk_hWindowFramePipelineLayout,
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
					offsetof(WindowShaderData, u32WindowFrameToRender),
					sizeof(WindowShaderData::u32WindowFrameToRender),
					static_cast<const void*>(&windowPushConstants));
			vkCmdDraw(vk_hCommandBuffer, 4, 6, 0, 0);
			windowPushConstants.u32WindowFrameToRender = WINDOW_FRAME_RENDER_MODE_TITLE;
			vkCmdPushConstants(
					vk_hCommandBuffer,
					vk_hWindowFramePipelineLayout,
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
					offsetof(WindowShaderData, u32WindowFrameToRender),
					sizeof(WindowShaderData::u32WindowFrameToRender),
					static_cast<const void*>(&windowPushConstants));
			vkCmdDrawIndirect(
					vk_hCommandBuffer,
					vk_hWindowFrameBuffer,
					sizeof(WindowFrameUniformData),
					1,
					sizeof(VkDrawIndirectCommand));
		}
	}

#endif

}
