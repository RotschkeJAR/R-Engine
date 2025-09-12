#include "RE_Renderer_Internal.hpp"
#include "RE_Window.hpp"
#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_Sub Renderer.hpp"
#include "RE_Render System.hpp"

#include <thread>

namespace RE {

	typedef uint16_t REindex_t;
#define RE_VK_RECT_INDEX_BUFFER_SIZE (RE_VK_RENDERABLE_RECTANGLES_COUNT * 6)
#define RE_VK_RECT_INDEX_BUFFER_SIZE_BYTES (RE_VK_RECT_INDEX_BUFFER_SIZE * sizeof(REindex_t))

#define RE_VK_VIEW_MATRIX_SIZE (4 * 4)
#define RE_VK_VIEW_MATRIX_OFFSET 0
#define RE_VK_PROJECTION_MATRIX_SIZE (4 * 4)
#define RE_VK_PROJECTION_MATRIX_OFFSET RE_VK_VIEW_MATRIX_SIZE

#define RE_VK_CAMERA_UNIFORM_BUFFER_SIZE (RE_VK_VIEW_MATRIX_SIZE + RE_VK_PROJECTION_MATRIX_SIZE)
#define RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES (RE_VK_CAMERA_UNIFORM_BUFFER_SIZE * sizeof(float))

#define RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE 2
#define RE_VK_SWAPCHAIN_SEMAPHORE_COUNT (u32SwapchainImageCount * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE)

	const Camera *pActiveCamera = nullptr;

	ScreenPercentageSettings screenPercentageSettings(RE_SCREEN_PERCENTAGE_MODE_NORMAL);

	VkBool32 vk_bSampleShadingEnabled = VK_FALSE;
	float fSampleShadingRate = 0.2f;

	std::array<VulkanTask, RE_VK_FRAMES_IN_FLIGHT> renderTasks;
	std::array<VkFence, renderTasks.size()> renderFences;
	std::vector<VkSemaphore> swapchainSemaphores;

	uint32_t u32NextSwapchainSemaphoreIndex = 0;
	uint8_t u8CurrentFrameInFlightIndex = 0;

	uint8_t get_render_graphics_queue_logical_index() {
		return renderTasks[0].get_logical_queue_index_for_function(1);
	}

	bool init_renderer() {
		constexpr VkQueueFlagBits vk_aeQueuesUsedForRendering[] = {
			VK_QUEUE_COMPUTE_BIT,
			VK_QUEUE_GRAPHICS_BIT
		};
		renderTasks[0].init(sizeof(vk_aeQueuesUsedForRendering) / sizeof(vk_aeQueuesUsedForRendering[0]), vk_aeQueuesUsedForRendering, false);
		for (uint32_t u32RenderTaskIndex = 1; u32RenderTaskIndex < renderTasks.size(); u32RenderTaskIndex++)
			renderTasks[u32RenderTaskIndex].init(renderTasks[0]);
		const VkFenceCreateInfo vk_fenceCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT
		};
		uint8_t u8CurrentFenceIndex = 0;
		for (VkFence &vk_rhFence : renderFences)
			if (vkCreateFence(vk_hDevice, &vk_fenceCreateInfo, nullptr, &vk_rhFence) != VK_SUCCESS)
				break;
			else
				u8CurrentFenceIndex++;
		if (u8CurrentFenceIndex == static_cast<uint8_t>(renderFences.size())) {
			Vulkan_Buffer stagingIndexBuffer(RE_VK_INDEX_BUFFER_SIZE, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 1, nullptr, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			VulkanTask indexBufferTransferTask;
			Vulkan_TimelineSemaphore indexBufferTransferTimelineSemaphore;
			std::thread threadIndexBufferDataInit;
			if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_rect_index_buffer(stagingIndexBuffer.get_buffer(), stagingIndexBuffer.get_memory(), indexBufferTransferTask, indexBufferTransferTimelineSemaphore, threadIndexBufferDataInit), bool)) {
				VulkanTask depthImageLayoutTransitionTask;
				Vulkan_Fence depthStencilImageLayoutTransitionFence;
				if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_depth_stencil_buffers(depthImageLayoutTransitionTask, depthStencilImageLayoutTransitionFence.get_fence()), bool)) {
					if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_descriptor_sets(), bool)) {
						if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_renderpass(), bool)) {
							if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(init_sub_renderers(), bool)) {
								threadIndexBufferDataInit.join();
								indexBufferTransferTimelineSemaphore.wait_for_reaching(RE_VK_TIMELINE_SEMAPHORE_FINISH);
								depthStencilImageLayoutTransitionFence.wait_for();
								return true;
							}
							PUSH_TO_CALLSTACKTRACE(destroy_renderpass());
						}
						PUSH_TO_CALLSTACKTRACE(destroy_descriptor_sets());
					}
					depthStencilImageLayoutTransitionFence.wait_for();
					PUSH_TO_CALLSTACKTRACE(destroy_depth_stencil_buffers());
				}
				threadIndexBufferDataInit.join();
				indexBufferTransferTimelineSemaphore.wait_for_reaching(RE_VK_TIMELINE_SEMAPHORE_FINISH);
				PUSH_TO_CALLSTACKTRACE(destroy_rect_index_buffer());
			}
		} else
			RE_FATAL_ERROR("Failed to create Vulkan fence at index ", u8CurrentFenceIndex, " to wait on rendering to finish");
		for (uint8_t u8FenceDestroyIndex = 0; u8FenceDestroyIndex < u8CurrentFenceIndex; u8FenceDestroyIndex++)
			vkDestroyFence(vk_hDevice, renderFences[u8FenceDestroyIndex], nullptr);
		for (VulkanTask &rRenderTask : renderTasks)
			rRenderTask.destroy();
		return false;
	}
	
	void destroy_renderer() {
		PUSH_TO_CALLSTACKTRACE(destroy_sub_renderers());
		PUSH_TO_CALLSTACKTRACE(destroy_renderpass());
		PUSH_TO_CALLSTACKTRACE(destroy_descriptor_sets());
		PUSH_TO_CALLSTACKTRACE(destroy_depth_stencil_buffers());
		PUSH_TO_CALLSTACKTRACE(destroy_rect_index_buffer());
		for (const VkFence &vk_rhFence : renderFences)
			vkDestroyFence(vk_hDevice, vk_rhFence, nullptr);
		for (VulkanTask &rRenderTask : renderTasks)
			rRenderTask.destroy();
	}

	void render() {

	}

	bool swapchain_created_renderer() {
		return true;
	}

	void swapchain_destroyed_renderer() {
		
	}

	void attach_camera(const Camera *const pCamera) {
		pActiveCamera = pCamera;
	}

	void set_screen_percentage_settings(const ScreenPercentageSettings &rNewSettings) {
		bool bRequiresRecreatingRenderPass = false;
		switch (rNewSettings.eMode) {
			case RE_SCREEN_PERCENTAGE_MODE_NORMAL:
				if (screenPercentageSettings.eMode == RE_SCREEN_PERCENTAGE_MODE_NORMAL)
					return;
				bRequiresRecreatingRenderPass = true;
				break;
			case RE_SCREEN_PERCENTAGE_MODE_SCALED:
				if (screenPercentageSettings.eMode == RE_SCREEN_PERCENTAGE_MODE_SCALED && screenPercentageSettings.fScale == rNewSettings.fScale)
					return;
				bRequiresRecreatingRenderPass = screenPercentageSettings.eMode == RE_SCREEN_PERCENTAGE_MODE_NORMAL;
				break;
			case RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE:
				if (screenPercentageSettings.eMode == RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE && screenPercentageSettings.constSize == rNewSettings.constSize)
					return;
				bRequiresRecreatingRenderPass = screenPercentageSettings.eMode == RE_SCREEN_PERCENTAGE_MODE_NORMAL;
				break;
		}
		if (vk_hDevice == VK_NULL_HANDLE) {
			screenPercentageSettings = rNewSettings;
			return;
		}
		WAIT_FOR_IDLE_VULKAN_DEVICE();
		screenPercentageSettings = rNewSettings;
	}

	[[nodiscard]]
	ScreenPercentageSettings get_screen_percentage_settings() {
		return screenPercentageSettings;
	}

	void enable_sample_shading(const bool bEnable) {
		if (is_sample_shading_enabled() == bEnable)
			return;
		vk_bSampleShadingEnabled = bEnable ? VK_TRUE : VK_FALSE;
		if (vk_hDevice == VK_NULL_HANDLE)
			return;
		WAIT_FOR_IDLE_VULKAN_DEVICE();
	}

	[[nodiscard]]
	bool is_sample_shading_enabled() {
		return vk_bSampleShadingEnabled == VK_TRUE;
	}

	void set_sample_shading_rate(const float fNewSampleShadingRate) {
		if (fSampleShadingRate == fNewSampleShadingRate)
			return;
		if (fNewSampleShadingRate < 0.0f || fNewSampleShadingRate > 1.0f) {
			RE_ERROR("Sample shading rate should be in range between 0 and 1, but was ", fNewSampleShadingRate, ". Request to change it has been discarded");
			return;
		} else if (fNewSampleShadingRate == 0.0f) {
			RE_WARNING("Sample shading rate of zero disables sample shading. The engine will ignore this new rate and disable it instead");
			vk_bSampleShadingEnabled = VK_FALSE;
		} else
			fSampleShadingRate = fNewSampleShadingRate;
		if (vk_hDevice == VK_NULL_HANDLE)
			return;
		WAIT_FOR_IDLE_VULKAN_DEVICE();
	}

	[[nodiscard]]
	float get_sample_shading_rate() {
		return fSampleShadingRate;
	}

}
