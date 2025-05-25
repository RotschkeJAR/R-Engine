#include "RE_Renderer.hpp"
#include "RE_Internal Header.hpp"
#include "RE_Render System.hpp"
#include "RE_Window.hpp"

#include <thread>

namespace RE {

	typedef uint16_t REindex_t;
#define RE_VK_INDEX_BUFFER_SIZE RE_VK_RENDERABLE_RECTANGLES_COUNT * 6U
#define RE_VK_INDEX_BUFFER_BYTES RE_VK_INDEX_BUFFER_SIZE * sizeof(REindex_t)

	Camera *pActiveCamera = nullptr;
	VkViewport vk_cameraViewportArea;
	VkRect2D vk_cameraScissorArea;

	VkBuffer vk_hRectIndexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory vk_hRectIndexBufferMemory = VK_NULL_HANDLE;

	bool init_renderer() {
		constexpr uint32_t u32StagingRectIndexBufferQueueCount = 1U, u32StagingRectIndexBufferQueues[u32StagingRectIndexBufferQueueCount] = {RE_VK_QUEUE_TRANSFER_INDEX};
		Vulkan_Buffer stagingRectIndexBuffer(RE_VK_INDEX_BUFFER_BYTES, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, u32StagingRectIndexBufferQueueCount, u32StagingRectIndexBufferQueues, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		if (!stagingRectIndexBuffer.is_valid())
			return false;
		uint16_t *pu16RectIndices;
		stagingRectIndexBuffer.map_memory((void**)&pu16RectIndices);
		std::thread initRectIndexBufferData([&]() {
			for (uint16_t u16RectNumber = 0U; u16RectNumber < RE_VK_RENDERABLE_RECTANGLES_COUNT; u16RectNumber++) {
				pu16RectIndices[u16RectNumber * 6U + 0U] = u16RectNumber * 4U + 0U;
				pu16RectIndices[u16RectNumber * 6U + 1U] = u16RectNumber * 4U + 1U;
				pu16RectIndices[u16RectNumber * 6U + 2U] = u16RectNumber * 4U + 2U;
				pu16RectIndices[u16RectNumber * 6U + 3U] = u16RectNumber * 4U + 2U;
				pu16RectIndices[u16RectNumber * 6U + 4U] = u16RectNumber * 4U + 3U;
				pu16RectIndices[u16RectNumber * 6U + 5U] = u16RectNumber * 4U + 0U;
			}
		});
		constexpr uint32_t u32RectIndexBufferQueueCount = 2U, u32RectIndexBufferQueues[u32RectIndexBufferQueueCount] = {RE_VK_QUEUE_TRANSFER_INDEX, RE_VK_QUEUE_GRAPHICS_INDEX};
		vk_create_buffer(RE_VK_INDEX_BUFFER_BYTES, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, u32RectIndexBufferQueueCount, u32RectIndexBufferQueues, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_hRectIndexBuffer, &vk_hRectIndexBufferMemory);
		Vulkan_CommandBuffer transferRectIndicesCommandBuffer(vk_hCommandPools[RE_VK_COMMAND_POOL_TRANSFER_TRANSIENT_INDEX], VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		if (!transferRectIndicesCommandBuffer.is_valid() || !transferRectIndicesCommandBuffer.begin_recording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr)) {
			vk_destroy_buffer(vk_hRectIndexBuffer, vk_hRectIndexBufferMemory);
			initRectIndexBufferData.join();
			return false;
		}
		transferRectIndicesCommandBuffer.cmd_copy_buffers(stagingRectIndexBuffer, vk_hRectIndexBuffer, RE_VK_INDEX_BUFFER_BYTES);
		transferRectIndicesCommandBuffer.end_recording();
		Vulkan_Fence rectIndexBufferTransferFence;
		initRectIndexBufferData.join();
		if (!vk_submit_to_transfer_queue(0U, nullptr, nullptr, 1U, transferRectIndicesCommandBuffer, 0U, nullptr, rectIndexBufferTransferFence)) {
			vk_destroy_buffer(vk_hRectIndexBuffer, vk_hRectIndexBufferMemory);
			return false;
		}
		rectIndexBufferTransferFence.wait_for();
		return true;
	}
	
	void destroy_renderer() {
		vk_destroy_buffer(vk_hRectIndexBuffer, vk_hRectIndexBufferMemory);
	}

	void render() {
		if (pActiveCamera)
			CATCH_SIGNAL(pActiveCamera->update());
		// TODO: render process
	}

	void calculate_render_area() {
		if (pActiveCamera) {
			const float fCameraScale = std::min(vk_swapchainResolution.width / pActiveCamera->scale[0], vk_swapchainResolution.height / pActiveCamera->scale[1]);
			vk_cameraViewportArea.width = std::round(pActiveCamera->scale[0] * fCameraScale);
			vk_cameraViewportArea.height = std::round(pActiveCamera->scale[1] * fCameraScale);
			vk_cameraViewportArea.x = std::round((vk_swapchainResolution.width - vk_cameraViewportArea.width) / 2.0f);
			vk_cameraViewportArea.y = std::round((vk_swapchainResolution.height - vk_cameraViewportArea.height) / 2.0f);
		} else {
			vk_cameraViewportArea.width = vk_swapchainResolution.width;
			vk_cameraViewportArea.height = vk_swapchainResolution.height;
			vk_cameraViewportArea.x = 0.0f;
			vk_cameraViewportArea.y = 0.0f;
		}
		vk_cameraScissorArea.extent.width = vk_cameraViewportArea.width;
		vk_cameraScissorArea.extent.height = vk_cameraViewportArea.height;
		vk_cameraScissorArea.offset.x = vk_cameraViewportArea.x;
		vk_cameraScissorArea.offset.y = vk_cameraViewportArea.y;
	}

}
