#include "RE_Renderer.hpp"
#include "RE_Internal Header.hpp"
#include "RE_Render System.hpp"
#include "RE_Window.hpp"

namespace RE {

	typedef uint16_t REindex_t;
#define RE_VK_INDEX_BUFFER_SIZE RE_VK_RENDERABLE_RECTANGLES_COUNT * 6U
#define RE_VK_INDEX_BUFFER_BYTES RE_VK_INDEX_BUFFER_SIZE * sizeof(REindex_t)

	SubRenderer::SubRenderer() {
		// TODO: create secondary command buffers
	}

	SubRenderer::~SubRenderer() {
		// TODO: clear secondary buffers
	}

	bool SubRenderer::is_subrenderer_valid() const {
		return true;
	}



#define RE_VK_RENDER_PASS_ATTACHMENT_DESCRIPTION_COUNT 1U
#define RE_VK_RENDER_PASS_SUBPASS_DESCRIPTION_COUNT 1U
#define RE_VK_RENDER_PASS_SUBPASS_DEPENDENCY_COUNT 1U

	VkAttachmentDescription vk_colorAttachment[RE_VK_RENDER_PASS_ATTACHMENT_DESCRIPTION_COUNT] = {
		{
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		}
	};
	constexpr VkAttachmentReference vk_attachmentReference[] = {
		{
			.attachment = 0U,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		}
	};
	constexpr VkSubpassDescription vk_subpassDescription[RE_VK_RENDER_PASS_SUBPASS_DESCRIPTION_COUNT] = {
		{
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.colorAttachmentCount = 1U,
			.pColorAttachments = &vk_attachmentReference[0]
		}
	};
	constexpr VkSubpassDependency vk_subpassDependency[RE_VK_RENDER_PASS_SUBPASS_DEPENDENCY_COUNT] = {
		{
			.srcSubpass = VK_SUBPASS_EXTERNAL,
			.dstSubpass = 0,
			.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = 0U,
			.dstAccessMask = 0U
		}
	};

	constexpr uint32_t u32IndexBufferQueueTypes[] = {RE_VK_QUEUE_GRAPHICS_INDEX, RE_VK_QUEUE_TRANSFER_INDEX};
	Camera *pActiveCamera = nullptr;
	Renderer *Renderer::pInstance = nullptr;

	static uint32_t setup_renderpass_objects() {
		vk_colorAttachment[0].format = vk_eSwapchainImageFormat;
		return RE_VK_RENDER_PASS_ATTACHMENT_DESCRIPTION_COUNT;
	}

	Renderer::Renderer() 
#if (RE_VK_FRAMES_IN_FLIGHT == 2)
#else
# error Update the array initializations above!
#endif
	{
		if (pInstance) {
			RE_FATAL_ERROR("Another instance of Renderer has been constructed.");
			return;
		}
		pInstance = this;
		// TODO: create static buffers
		bValid = true;
	}

	Renderer::~Renderer() {
		if (pInstance != this)
			return;
		pInstance = nullptr;
		// TODO: clear memory
	}

	void Renderer::create_framebuffers() {
		// TODO: create framebuffers
	}

	void Renderer::destroy_framebuffers() {
		// TODO: destroy framebuffers
	}

	void Renderer::create_command_buffers() {
		// TODO: create command buffers
	}
	
	void Renderer::destroy_command_buffers() {
		// TODO: destroy command buffers
	}

	void Renderer::record_command_buffer() {
		// TODO: record primary command buffer
	}

	void Renderer::render() {
		if (pActiveCamera)
			CATCH_SIGNAL(pActiveCamera->update());
		// TODO: render proess
		u8CurrentFrameInFlight = (u8CurrentFrameInFlight + 1) % RE_VK_FRAMES_IN_FLIGHT;
	}

	void Renderer::swapchain_recreated() {
		// TODO: recreate framebuffers, command buffers
	}

	void Renderer::calculate_render_area() {
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

	void Renderer::wait_for_all_fences() const {
		// TODO: wait for fences
	}

	bool Renderer::is_valid() const {
		return bValid;
	}

}
