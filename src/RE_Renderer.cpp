#include "RE_Renderer.hpp"
#include "RE_Internal Header.hpp"
#include "RE_Window.hpp"
#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_Renderer_GameObject.hpp"
#include "RE_Renderer Texture.hpp"

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
	VkViewport vk_cameraViewport = {
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};
	VkRect2D vk_cameraScissor;
	VkExtent2D vk_worldRenderImageExtent2D;

	ScreenPercentageSettings screenPercentageSettings(RE_SCREEN_PERCENTAGE_MODE_NORMAL);

	VkBool32 vk_bSampleShadingEnabled = VK_FALSE;
	float fSampleShadingRate = 0.2f;

	VkDescriptorSetLayout vk_hWorldDescriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorPool vk_hWorldDescriptorSetPool = VK_NULL_HANDLE;
	std::array<VkDescriptorSet, RE_VK_TOTAL_COUNT_OF_DESCRIPTOR_SETS> vk_ahDescriptorSets = {};
	std::array<VkBuffer, RE_VK_FRAMES_IN_FLIGHT> vk_ahWorldCameraUniformBuffers = {};
	std::array<VkDeviceMemory, RE_VK_FRAMES_IN_FLIGHT> vk_ahWorldCameraUniformBufferMemories = {};
	std::array<float*, RE_VK_FRAMES_IN_FLIGHT> apafCameraUniformData = {};

	VkPipelineLayout vk_hWorldBasicPipelineLayout = VK_NULL_HANDLE;

	std::array<VkImage, RE_VK_FRAMES_IN_FLIGHT> vk_ahWorldDepthStencilImages = {};
	std::array<VkDeviceMemory, RE_VK_FRAMES_IN_FLIGHT> vk_ahWorldDepthStencilImageMemories = {};
	std::array<VkImageView, RE_VK_FRAMES_IN_FLIGHT> vk_ahWorldDepthStencilImageViews = {};

	VkRenderPass vk_hWorldRenderPass = VK_NULL_HANDLE;
	std::array<VkImage, RE_VK_FRAMES_IN_FLIGHT> vk_ahWorldRenderImages = {};
	std::array<VkDeviceMemory, RE_VK_FRAMES_IN_FLIGHT> vk_ahWorldRenderImageMemories = {};
	std::array<VkImageView, RE_VK_FRAMES_IN_FLIGHT> vk_ahWorldRenderImageViews = {};
	std::array<VkFramebuffer, RE_VK_FRAMES_IN_FLIGHT> vk_ahWorldFramebuffers = {};

	VkImage *vk_pahSwapchainDepthStencilImages = nullptr;
	VkDeviceMemory *vk_pahSwapchainDepthStencilImageMemories = nullptr;
	VkImageView *vk_pahSwapchainDepthStencilImageViews = nullptr;
	VkFramebuffer *vk_pahSwapchainFramebuffers = nullptr;

	std::array<VkFence, RE_VK_FRAMES_IN_FLIGHT> vk_ahRenderFences = {};
	VkSemaphore *vk_pahSwapchainSemaphores = nullptr;
	std::array<VkSemaphore, RE_VK_RENDER_SEMAPHORE_COUNT> vk_ahRenderSemaphores = {};

	std::array<VkCommandBuffer, RE_VK_FRAMES_IN_FLIGHT> vk_ahRenderCommandBuffers = {};

	uint32_t u32NextSwapchainSemaphoreIndex = 0;
	uint8_t u8CurrentFrameInFlightIndex = 0;

#define GET_VULKAN_MATRIX_ELEMENT_INDEX(COLUMN, LINE) (COLUMN * 4 + LINE)
#define IS_MSAA_ENABLED() (vk_eMsaaCount != VK_SAMPLE_COUNT_1_BIT)
#define IS_SCREEN_PERCENTAGE_MODIFIED() (screenPercentageSettings.eMode != RE_SCREEN_PERCENTAGE_MODE_NORMAL)
#define ARE_MSAA_AND_SCREEN_PERCENTAGE_MODIFIED() (IS_MSAA_ENABLED() && IS_SCREEN_PERCENTAGE_MODIFIED())
#define SHOULD_USE_SWAPCHAIN_IMAGES_DIRECTLY() (screenPercentageSettings.eMode == RE_SCREEN_PERCENTAGE_MODE_NORMAL && vk_eMsaaCount == VK_SAMPLE_COUNT_1_BIT)

	static bool create_render_pass() {
		/*constexpr uint32_t u32WorldRenderPassAttachmentCount = 2, u32WorldRenderPassSubpassCount = 1, u32WorldRenderPassDependencyCount = 1;
		const std::array<VkAttachmentDescription, u32WorldRenderPassAttachmentCount> vk_aWorldRenderPassAttachments = {{
			{
				.format = vk_eSwapchainImageFormat,
				.samples = vk_eMsaaCount,
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
				.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.finalLayout = SHOULD_USE_SWAPCHAIN_IMAGES_DIRECTLY() ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
			}, {
				.format = vk_eDepthStencilBufferFormat,
				.samples = vk_eMsaaCount,
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
				.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
			}
		}};
		const std::array<VkAttachmentReference, 1> vk_aWorldRenderPassColorAttachments = {{
			{
				.attachment = 0,
				.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			}
		}};
		const std::array<VkAttachmentReference, 1> vk_aWorldRenderPassDepthStencilAttachments = {{
			{
				.attachment = 1,
				.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
			}
		}};
		const std::array<VkSubpassDescription, u32WorldRenderPassSubpassCount> vk_aWorldRenderSubpasses = {{
			{
				.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
				.inputAttachmentCount = 0,
				.pInputAttachments = nullptr,
				.colorAttachmentCount = 1,
				.pColorAttachments = &vk_aWorldRenderPassColorAttachments[0],
				.pDepthStencilAttachment = &vk_aWorldRenderPassDepthStencilAttachments[0]
			}
		}};
		const std::array<VkSubpassDependency, u32WorldRenderPassDependencyCount> vk_aWorldRenderDependencies = {{
			{
				.srcSubpass = VK_SUBPASS_EXTERNAL,
				.dstSubpass = RE_VK_GAME_OBJECT_SUPBASS,
				.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
				.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
				.srcAccessMask = VK_ACCESS_NONE,
				.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				.dependencyFlags = 0
			}
		}};
		const VkRenderPassCreateInfo vk_renderPassCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.attachmentCount = u32WorldRenderPassAttachmentCount,
			.pAttachments = vk_aWorldRenderPassAttachments.data(),
			.subpassCount = u32WorldRenderPassSubpassCount,
			.pSubpasses = vk_aWorldRenderSubpasses.data(),
			.dependencyCount = u32WorldRenderPassDependencyCount,
			.pDependencies = vk_aWorldRenderDependencies.data()
		};
		if (vkCreateRenderPass(vk_hDevice, &vk_renderPassCreateInfo, nullptr, &vk_hWorldRenderPass) != VK_SUCCESS) {
			RE_FATAL_ERROR("Failed to create Vulkan render pass");
			return false;
		}*/
		return true;
	}

	static bool create_world_render_images() {
		/*switch (screenPercentageSettings.eMode) {
			case RE_SCREEN_PERCENTAGE_MODE_NORMAL:
				vk_worldRenderImageExtent2D = vk_swapchainResolution;
				break;
			case RE_SCREEN_PERCENTAGE_MODE_SCALED:
				vk_worldRenderImageExtent2D = {
					.width = static_cast<uint32_t>(std::round(vk_swapchainResolution.width * screenPercentageSettings.fScale)),
					.height = static_cast<uint32_t>(std::round(vk_swapchainResolution.height * screenPercentageSettings.fScale))
				};
				break;
			case RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE:
				vk_worldRenderImageExtent2D = {
					.width = screenPercentageSettings.constSize[0],
					.height = screenPercentageSettings.constSize[1]
				};
				break;
		}
		const VkExtent3D vk_renderImageExtent3D = {
			.width = vk_worldRenderImageExtent2D.width,
			.height = vk_worldRenderImageExtent2D.height,
			.depth = 1
		};
		constexpr uint32_t u32GraphicsQueueOnlyCount = 1;
		const std::array<uint32_t, u32GraphicsQueueOnlyCount> au32GraphicsQueueOnly = {{RE_VK_QUEUE_GRAPHICS_INDEX}};
		if (!SHOULD_USE_SWAPCHAIN_IMAGES_DIRECTLY()) {
			constexpr uint32_t u32WorldRenderImageQueueCount = 2;
			const std::array<uint32_t, u32WorldRenderImageQueueCount> au32WorldRenderImageQueues = {RE_VK_QUEUE_GRAPHICS_INDEX, RE_VK_QUEUE_TRANSFER_INDEX};
			uint8_t u8WorldRenderImageCollectionCreateIndex = 0;
			while (u8WorldRenderImageCollectionCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
				if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_image(0, VK_IMAGE_TYPE_2D, vk_eSwapchainImageFormat, vk_renderImageExtent3D, 1, 1, vk_eMsaaCount, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, u32WorldRenderImageQueueCount, au32WorldRenderImageQueues.data(), VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_ahWorldRenderImages[u8WorldRenderImageCollectionCreateIndex], &vk_ahWorldRenderImageMemories[u8WorldRenderImageCollectionCreateIndex]), bool)) {
					if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_image_view(vk_ahWorldRenderImages[u8WorldRenderImageCollectionCreateIndex], VK_IMAGE_VIEW_TYPE_2D, vk_eSwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1, &vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionCreateIndex]), bool)) {
						if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_image(0, VK_IMAGE_TYPE_2D, vk_eDepthStencilBufferFormat, vk_renderImageExtent3D, 1, 1, vk_eMsaaCount, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, u32GraphicsQueueOnlyCount, au32GraphicsQueueOnly.data(), VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_ahWorldDepthStencilImages[u8WorldRenderImageCollectionCreateIndex], &vk_ahWorldDepthStencilImageMemories[u8WorldRenderImageCollectionCreateIndex]), bool)) {
							if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_image_view(vk_ahWorldDepthStencilImages[u8WorldRenderImageCollectionCreateIndex], VK_IMAGE_VIEW_TYPE_2D, vk_eDepthStencilBufferFormat, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, 0, 1, &vk_ahWorldDepthStencilImageViews[u8WorldRenderImageCollectionCreateIndex]), bool)) {
								VkCommandBuffer vk_hDepthStencilBufferTransitCommandBuffer;
								if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(transit_image(RE_VK_QUEUE_GRAPHICS_INDEX, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, VK_ACCESS_NONE, VK_ACCESS_NONE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, vk_ahWorldDepthStencilImages[u8WorldRenderImageCollectionCreateIndex], VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, 0, 1, &vk_hDepthStencilBufferTransitCommandBuffer, nullptr), bool)) {
									constexpr uint32_t u32FramebufferImageViewCount = 2;
									const std::array<VkImageView, u32FramebufferImageViewCount> vk_ahFramebufferImageViews = {{vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionCreateIndex], vk_ahWorldDepthStencilImageViews[u8WorldRenderImageCollectionCreateIndex]}};
									if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_framebuffer(0, vk_hWorldRenderPass, u32FramebufferImageViewCount, vk_ahFramebufferImageViews.data(), vk_renderImageExtent3D.width, vk_renderImageExtent3D.height, 1, &vk_ahWorldFramebuffers[u8WorldRenderImageCollectionCreateIndex]), bool)) {
										if (ARE_MSAA_AND_SCREEN_PERCENTAGE_MODIFIED() && !PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_image(0, VK_IMAGE_TYPE_2D, vk_eSwapchainImageFormat, vk_renderImageExtent3D, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, u32GraphicsQueueOnlyCount, au32GraphicsQueueOnly.data(), VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_ahSingleSampledWorldRenderImages[u8WorldRenderImageCollectionCreateIndex], &vk_ahSingleSampledWorldRenderImageMemories[u8WorldRenderImageCollectionCreateIndex]), bool))
											RE_FATAL_ERROR("Failed to create Vulkan image at index ", u8WorldRenderImageCollectionCreateIndex, " to store single sampled image of the rendered world");
										else {
											u8WorldRenderImageCollectionCreateIndex++;
											continue;
										}
									} else
										RE_FATAL_ERROR("Failed to create Vulkan framebuffer at index ", u8WorldRenderImageCollectionCreateIndex, " for world rendering");
								} else
									RE_FATAL_ERROR("Failed to transit depth & stencil buffer at index ", u8WorldRenderImageCollectionCreateIndex, " to its required image layout");
								vkDestroyImageView(vk_hDevice, vk_ahWorldDepthStencilImageViews[u8WorldRenderImageCollectionCreateIndex], nullptr);
							} else
								RE_FATAL_ERROR("Failed to create Vulkan image view for the depth & stencil buffer at index ", u8WorldRenderImageCollectionCreateIndex);
							vkFreeMemory(vk_hDevice, vk_ahWorldDepthStencilImageMemories[u8WorldRenderImageCollectionCreateIndex], nullptr);
							vkDestroyImage(vk_hDevice, vk_ahWorldDepthStencilImages[u8WorldRenderImageCollectionCreateIndex], nullptr);
						} else
							RE_FATAL_ERROR("Failed to create Vulkan image for depth & stencil buffer usage at index ", u8WorldRenderImageCollectionCreateIndex);
						vkDestroyImageView(vk_hDevice, vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionCreateIndex], nullptr);
					} else
						RE_FATAL_ERROR("Failed to create Vulkan GPU-side image view at index ", u8WorldRenderImageCollectionCreateIndex, " for world rendering");
					vkFreeMemory(vk_hDevice, vk_ahWorldRenderImageMemories[u8WorldRenderImageCollectionCreateIndex], nullptr);
					vkDestroyImage(vk_hDevice, vk_ahWorldRenderImages[u8WorldRenderImageCollectionCreateIndex], nullptr);
				} else
					RE_FATAL_ERROR("Failed to create Vulkan GPU-side image at index ", u8WorldRenderImageCollectionCreateIndex, " for world rendering");
				vk_ahSingleSampledWorldRenderImageMemories[u8WorldRenderImageCollectionCreateIndex] = VK_NULL_HANDLE;
				vk_ahSingleSampledWorldRenderImages[u8WorldRenderImageCollectionCreateIndex] = VK_NULL_HANDLE;
				vk_ahWorldDepthStencilImageViews[u8WorldRenderImageCollectionCreateIndex] = VK_NULL_HANDLE;
				vk_ahWorldDepthStencilImageMemories[u8WorldRenderImageCollectionCreateIndex] = VK_NULL_HANDLE;
				vk_ahWorldDepthStencilImages[u8WorldRenderImageCollectionCreateIndex] = VK_NULL_HANDLE;
				vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionCreateIndex] = VK_NULL_HANDLE;
				vk_ahWorldRenderImageMemories[u8WorldRenderImageCollectionCreateIndex] = VK_NULL_HANDLE;
				vk_ahWorldRenderImages[u8WorldRenderImageCollectionCreateIndex] = VK_NULL_HANDLE;
				break;
			}
			if (u8WorldRenderImageCollectionCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
				return true;
			for (uint8_t u8WorldRenderImageCollectionDeleteIndex = 0; u8WorldRenderImageCollectionDeleteIndex < u8WorldRenderImageCollectionCreateIndex; u8WorldRenderImageCollectionDeleteIndex++) {
				if (ARE_MSAA_AND_SCREEN_PERCENTAGE_MODIFIED()) {
					vkFreeMemory(vk_hDevice, vk_ahSingleSampledWorldRenderImageMemories[u8WorldRenderImageCollectionDeleteIndex], nullptr);
					vkDestroyImage(vk_hDevice, vk_ahSingleSampledWorldRenderImages[u8WorldRenderImageCollectionDeleteIndex], nullptr);
				}
				vkDestroyFramebuffer(vk_hDevice, vk_ahWorldFramebuffers[u8WorldRenderImageCollectionDeleteIndex], nullptr);
				vkDestroyImageView(vk_hDevice, vk_ahWorldDepthStencilImageViews[u8WorldRenderImageCollectionDeleteIndex], nullptr);
				vkFreeMemory(vk_hDevice, vk_ahWorldDepthStencilImageMemories[u8WorldRenderImageCollectionDeleteIndex], nullptr);
				vkDestroyImage(vk_hDevice, vk_ahWorldDepthStencilImages[u8WorldRenderImageCollectionDeleteIndex], nullptr);
				vkDestroyImageView(vk_hDevice, vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionDeleteIndex], nullptr);
				vkFreeMemory(vk_hDevice, vk_ahWorldRenderImageMemories[u8WorldRenderImageCollectionDeleteIndex], nullptr);
				vkDestroyImage(vk_hDevice, vk_ahWorldRenderImages[u8WorldRenderImageCollectionDeleteIndex], nullptr);
				vk_ahSingleSampledWorldRenderImageMemories[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
				vk_ahSingleSampledWorldRenderImages[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
				vk_ahWorldFramebuffers[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
				vk_ahWorldDepthStencilImageViews[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
				vk_ahWorldDepthStencilImageMemories[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
				vk_ahWorldDepthStencilImages[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
				vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
				vk_ahWorldRenderImageMemories[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
				vk_ahWorldRenderImages[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
			}
		} else {
			vk_pahSwapchainDepthStencilImages = new VkImage[u32SwapchainImageCount];
			vk_pahSwapchainDepthStencilImageMemories = new VkDeviceMemory[u32SwapchainImageCount];
			vk_pahSwapchainDepthStencilImageViews = new VkImageView[u32SwapchainImageCount];
			vk_pahSwapchainFramebuffers = new VkFramebuffer[u32SwapchainImageCount];
			uint32_t u32SwapchainRenderImageCreateIndex = 0;
			while (u32SwapchainRenderImageCreateIndex < u32SwapchainImageCount) {
				if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_image(0, VK_IMAGE_TYPE_2D, vk_eDepthStencilBufferFormat, vk_renderImageExtent3D, 1, 1, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, u32GraphicsQueueOnlyCount, au32GraphicsQueueOnly.data(), VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_pahSwapchainDepthStencilImages[u32SwapchainRenderImageCreateIndex], &vk_pahSwapchainDepthStencilImageMemories[u32SwapchainRenderImageCreateIndex]), bool)) {
					if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_image_view(vk_pahSwapchainDepthStencilImages[u32SwapchainRenderImageCreateIndex], VK_IMAGE_VIEW_TYPE_2D, vk_eDepthStencilBufferFormat, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, 0, 1, &vk_pahSwapchainDepthStencilImageViews[u32SwapchainRenderImageCreateIndex]), bool)) {
						VkCommandBuffer vk_hDepthStencilBufferTransitCommandBuffer;
						if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(transit_image(RE_VK_QUEUE_GRAPHICS_INDEX, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, VK_ACCESS_NONE, VK_ACCESS_NONE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, vk_pahSwapchainDepthStencilImages[u32SwapchainRenderImageCreateIndex], VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, 0, 1, &vk_hDepthStencilBufferTransitCommandBuffer, nullptr), bool)) {
							constexpr uint32_t u32SwapchainFramebufferImageViewCount = 2;
							const std::array<VkImageView, u32SwapchainFramebufferImageViewCount> vk_ahSwapchainFramebufferImageViews = {{vk_pahSwapchainImageViews[u32SwapchainRenderImageCreateIndex], vk_pahSwapchainDepthStencilImageViews[u32SwapchainRenderImageCreateIndex]}};
							if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_framebuffer(0, vk_hWorldRenderPass, u32SwapchainFramebufferImageViewCount, vk_ahSwapchainFramebufferImageViews.data(), vk_renderImageExtent3D.width, vk_renderImageExtent3D.height, 1, &vk_pahSwapchainFramebuffers[u32SwapchainRenderImageCreateIndex]), bool)) {
								u32SwapchainRenderImageCreateIndex++;
								continue;
							} else
								RE_FATAL_ERROR("Failed to create Vulkan framebuffer at index ", u32SwapchainRenderImageCreateIndex, " for world rendering into swapchain image");
						} else
							RE_FATAL_ERROR("Failed to transit depth & stencil buffer at index ", u32SwapchainRenderImageCreateIndex, " to its required image layout");
						vkDestroyImageView(vk_hDevice, vk_pahSwapchainDepthStencilImageViews[u32SwapchainRenderImageCreateIndex], nullptr);
					} else
						RE_FATAL_ERROR("Failed to create Vulkan image view for the depth & stencil buffer at index ", u32SwapchainRenderImageCreateIndex);
					vkFreeMemory(vk_hDevice, vk_pahSwapchainDepthStencilImageMemories[u32SwapchainRenderImageCreateIndex], nullptr);
					vkDestroyImage(vk_hDevice, vk_pahSwapchainDepthStencilImages[u32SwapchainRenderImageCreateIndex], nullptr);
				} else
					RE_FATAL_ERROR("Failed to create Vulkan image for depth & stencil buffer usage at index ", u32SwapchainRenderImageCreateIndex);
			}
			if (u32SwapchainRenderImageCreateIndex == u32SwapchainImageCount)
				return true;
			for (uint32_t u32SwapchainRenderImageDeleteIndex = 0; u32SwapchainRenderImageDeleteIndex < u32SwapchainImageCount; u32SwapchainRenderImageDeleteIndex++) {
				vkDestroyFramebuffer(vk_hDevice, vk_pahSwapchainFramebuffers[u32SwapchainRenderImageDeleteIndex], nullptr);
				vkDestroyImageView(vk_hDevice, vk_pahSwapchainDepthStencilImageViews[u32SwapchainRenderImageDeleteIndex], nullptr);
				vkFreeMemory(vk_hDevice, vk_pahSwapchainDepthStencilImageMemories[u32SwapchainRenderImageDeleteIndex], nullptr);
				vkDestroyImage(vk_hDevice, vk_pahSwapchainDepthStencilImages[u32SwapchainRenderImageDeleteIndex], nullptr);
			}
			DELETE_ARRAY_SAFELY(vk_pahSwapchainDepthStencilImages);
			DELETE_ARRAY_SAFELY(vk_pahSwapchainDepthStencilImageMemories);
			DELETE_ARRAY_SAFELY(vk_pahSwapchainDepthStencilImageViews);
			DELETE_ARRAY_SAFELY(vk_pahSwapchainFramebuffers);
		}*/
		return false;
	}

	static void destroy_world_render_images() {
		/*if (!SHOULD_USE_SWAPCHAIN_IMAGES_DIRECTLY()) {
			if (vk_ahWorldRenderImages[0] == VK_NULL_HANDLE)
				return;
			for (uint32_t u8WorldRenderImageCollectionDeleteIndex = 0; u8WorldRenderImageCollectionDeleteIndex < RE_VK_FRAMES_IN_FLIGHT; u8WorldRenderImageCollectionDeleteIndex++) {
				if (ARE_MSAA_AND_SCREEN_PERCENTAGE_MODIFIED()) {
					vkFreeMemory(vk_hDevice, vk_ahSingleSampledWorldRenderImageMemories[u8WorldRenderImageCollectionDeleteIndex], nullptr);
					vkDestroyImage(vk_hDevice, vk_ahSingleSampledWorldRenderImages[u8WorldRenderImageCollectionDeleteIndex], nullptr);
				}
				vkDestroyFramebuffer(vk_hDevice, vk_ahWorldFramebuffers[u8WorldRenderImageCollectionDeleteIndex], nullptr);
				vkDestroyImageView(vk_hDevice, vk_ahWorldDepthStencilImageViews[u8WorldRenderImageCollectionDeleteIndex], nullptr);
				vkFreeMemory(vk_hDevice, vk_ahWorldDepthStencilImageMemories[u8WorldRenderImageCollectionDeleteIndex], nullptr);
				vkDestroyImage(vk_hDevice, vk_ahWorldDepthStencilImages[u8WorldRenderImageCollectionDeleteIndex], nullptr);
				vkDestroyImageView(vk_hDevice, vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionDeleteIndex], nullptr);
				vkFreeMemory(vk_hDevice, vk_ahWorldRenderImageMemories[u8WorldRenderImageCollectionDeleteIndex], nullptr);
				vkDestroyImage(vk_hDevice, vk_ahWorldRenderImages[u8WorldRenderImageCollectionDeleteIndex], nullptr);
				vk_ahSingleSampledWorldRenderImageMemories[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
				vk_ahSingleSampledWorldRenderImages[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
				vk_ahWorldFramebuffers[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
				vk_ahWorldDepthStencilImageViews[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
				vk_ahWorldDepthStencilImageMemories[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
				vk_ahWorldDepthStencilImages[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
				vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
				vk_ahWorldRenderImageMemories[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
				vk_ahWorldRenderImages[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
			}
		} else {
			if (!vk_pahSwapchainDepthStencilImages)
				return;
			for (uint32_t u32SwapchainImageIndex = 0; u32SwapchainImageIndex < u32SwapchainImageCount; u32SwapchainImageIndex++) {
				vkDestroyFramebuffer(vk_hDevice, vk_pahSwapchainFramebuffers[u32SwapchainImageIndex], nullptr);
				vkDestroyImageView(vk_hDevice, vk_pahSwapchainDepthStencilImageViews[u32SwapchainImageIndex], nullptr);
				vkFreeMemory(vk_hDevice, vk_pahSwapchainDepthStencilImageMemories[u32SwapchainImageIndex], nullptr);
				vkDestroyImage(vk_hDevice, vk_pahSwapchainDepthStencilImages[u32SwapchainImageIndex], nullptr);
			}
			DELETE_ARRAY_SAFELY(vk_pahSwapchainDepthStencilImages);
			DELETE_ARRAY_SAFELY(vk_pahSwapchainDepthStencilImageMemories);
			DELETE_ARRAY_SAFELY(vk_pahSwapchainDepthStencilImageViews);
			DELETE_ARRAY_SAFELY(vk_pahSwapchainFramebuffers);
		}*/
	}

	static void calculate_world_render_area_with_camera() {
		/*DEFINE_SIGNAL_GUARD(sigCamAccess);
		const std::array<float, 2> a2fCamScale = {
			{
				std::abs(pActiveCamera->view[0]),
				std::abs(pActiveCamera->view[1])
			}
		};
		const float fCameraScale = std::abs(std::min(vk_worldRenderImageExtent2D.width / a2fCamScale[0], vk_worldRenderImageExtent2D.height / a2fCamScale[1]));
		vk_cameraViewport.width = a2fCamScale[0] * fCameraScale;
		vk_cameraViewport.height = a2fCamScale[1] * fCameraScale;
		vk_cameraViewport.x = (vk_worldRenderImageExtent2D.width - vk_cameraViewport.width) / 2.0f;
		vk_cameraViewport.y = (vk_worldRenderImageExtent2D.height - vk_cameraViewport.height) / 2.0f;
		vk_cameraScissor.offset.x = static_cast<int32_t>(std::round(vk_cameraViewport.x));
		vk_cameraScissor.offset.y = static_cast<int32_t>(std::round(vk_cameraViewport.y));
		vk_cameraScissor.extent.width = static_cast<int32_t>(std::round(vk_cameraViewport.width));
		vk_cameraScissor.extent.height = static_cast<int32_t>(std::round(vk_cameraViewport.height));*/
	}

	static void update_camera_descriptor_set(const uint8_t u8FrameInFlightIndex) {
		/*const VkDescriptorBufferInfo vk_cameraUniformBufferToDescriptorSet = {
			.buffer = vk_ahWorldCameraUniformBuffers[u8FrameInFlightIndex],
			.offset = 0,
			.range = RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES
		};
		const VkWriteDescriptorSet vk_writeToCameraDescriptorSet = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = vk_ahDescriptorSets[u8FrameInFlightIndex],
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pBufferInfo = &vk_cameraUniformBufferToDescriptorSet
		};
		vkUpdateDescriptorSets(vk_hDevice, 1, &vk_writeToCameraDescriptorSet, 0, nullptr);*/
	}

	static void update_camera_uniform_buffer() {
		/*DEFINE_SIGNAL_GUARD(sigCamAccess);
		apafCameraUniformData[u8CurrentFrameInFlightIndex][RE_VK_VIEW_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(3, 0)] = -pActiveCamera->position[0];
		apafCameraUniformData[u8CurrentFrameInFlightIndex][RE_VK_VIEW_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(3, 1)] = -pActiveCamera->position[1];
		apafCameraUniformData[u8CurrentFrameInFlightIndex][RE_VK_VIEW_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(3, 2)] = -pActiveCamera->position[2];
		const float a2fCamScale[2] = {
			std::abs(pActiveCamera->view[0]) / 2.0f,
			std::abs(pActiveCamera->view[1]) / 2.0f
		};
		const float fLeft = pActiveCamera->position[0] - a2fCamScale[0],
			fRight = pActiveCamera->position[0] + a2fCamScale[0],
			fTop = pActiveCamera->position[1] + a2fCamScale[1],
			fBottom = pActiveCamera->position[1] - a2fCamScale[1],
			fNear = 10.0f,
			fFar = -fNear;
		apafCameraUniformData[u8CurrentFrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(0, 0)] = 2.0f / (fRight - fLeft) * sign(pActiveCamera->view[0]);
		apafCameraUniformData[u8CurrentFrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(1, 1)] = -2.0f / (fTop - fBottom) * sign(pActiveCamera->view[1]);
		apafCameraUniformData[u8CurrentFrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(2, 2)] = 1.0f / (fNear - fFar);
		/*apafCameraUniformData[u8CurrentFrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(3, 0)] = -(fRight + fLeft) / (fRight - fLeft);
		apafCameraUniformData[u8CurrentFrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(3, 1)] = -(fTop + fBottom) / (fTop - fBottom);
		apafCameraUniformData[u8CurrentFrameInFlightIndex][RE_VK_PROJECTION_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(3, 2)] = fNear / (fFar - fNear);*/
		PUSH_TO_CALLSTACKTRACE(update_camera_descriptor_set(u8CurrentFrameInFlightIndex));
	}

	bool init_renderer() {
		/*if (vk_bSampleShadingEnabled == VK_TRUE && vk_physicalDeviceFeatures.sampleRateShading == VK_FALSE) {
			vk_bSampleShadingEnabled = VK_FALSE;
			RE_WARNING("Sample shading has been disabled, because it's not supported on the selected GPU");
		}

		constexpr uint32_t u32StagingIndexBufferQueueCount = 1U, au32StagingIndexBufferQueues[u32StagingIndexBufferQueueCount] = {RE_VK_QUEUE_TRANSFER_INDEX};
		Vulkan_Buffer stagingIndexBuffer(RE_VK_RECT_INDEX_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, u32StagingIndexBufferQueueCount, au32StagingIndexBufferQueues, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		if (stagingIndexBuffer.is_valid()) {
			std::thread stagingIndexBufferDataInit([&]() {
				REindex_t *pau16RectIndices;
				stagingIndexBuffer.map_memory(reinterpret_cast<void**>(&pau16RectIndices));
				for (uint16_t u16RectNumber = 0U; u16RectNumber < RE_VK_RENDERABLE_RECTANGLES_COUNT; u16RectNumber++) {
					pau16RectIndices[u16RectNumber * 6U + 0U] = u16RectNumber * 4U + 0U;
					pau16RectIndices[u16RectNumber * 6U + 1U] = u16RectNumber * 4U + 1U;
					pau16RectIndices[u16RectNumber * 6U + 2U] = u16RectNumber * 4U + 2U;
					pau16RectIndices[u16RectNumber * 6U + 3U] = u16RectNumber * 4U + 2U;
					pau16RectIndices[u16RectNumber * 6U + 4U] = u16RectNumber * 4U + 3U;
					pau16RectIndices[u16RectNumber * 6U + 5U] = u16RectNumber * 4U + 0U;
				}
				stagingIndexBuffer.unmap_memory();
			});
			constexpr uint32_t u32IndexBufferQueueCount = 2, au32IndexBufferQueues[u32IndexBufferQueueCount] = {RE_VK_QUEUE_TRANSFER_INDEX, RE_VK_QUEUE_GRAPHICS_INDEX};
			if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_buffer(RE_VK_RECT_INDEX_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, u32IndexBufferQueueCount, au32IndexBufferQueues, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_hRectIndexBuffer, &vk_hRectIndexBufferMemory), bool)) {
				Vulkan_CommandBuffer indexBufferDataTransferCommandBuffer(vk_ahCommandPools[RE_VK_COMMAND_POOL_TRANSFER_TRANSIENT_INDEX], VK_COMMAND_BUFFER_LEVEL_PRIMARY);
				if (indexBufferDataTransferCommandBuffer.is_valid()) {
					if (indexBufferDataTransferCommandBuffer.begin_recording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr)) {
						constexpr VkBufferCopy vk_rectIndexBufferCopy = {
							.size = RE_VK_RECT_INDEX_BUFFER_SIZE_BYTES
						};
						vkCmdCopyBuffer(indexBufferDataTransferCommandBuffer, stagingIndexBuffer, vk_hRectIndexBuffer, 1, &vk_rectIndexBufferCopy);
						if (indexBufferDataTransferCommandBuffer.end_recording()) {
							Vulkan_Fence rectIndexBufferDataTransferFence;
							if (rectIndexBufferDataTransferFence.is_valid()) {
								constexpr uint32_t u32DescriptorSetLayoutBindingCount = 2;
								const std::array<VkDescriptorSetLayoutBinding, u32DescriptorSetLayoutBindingCount> vk_aDescriptorSetLayoutBindingInfos = {
									{
										{
											.binding = 0,
											.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
											.descriptorCount = RE_VK_COUNT_OF_CAMERA_UNIFORM_DESCRIPTOR,
											.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
											.pImmutableSamplers = nullptr
										}, {
											.binding = 1,
											.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
											.descriptorCount = RE_VK_COUNT_OF_TEXTURE_DESCRIPTOR,
											.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
											.pImmutableSamplers = nullptr
										}
									}
								};
								if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_descriptor_set_layout(u32DescriptorSetLayoutBindingCount, vk_aDescriptorSetLayoutBindingInfos.data(), 0, &vk_hWorldDescriptorSetLayout), bool)) {
									constexpr uint32_t u32DescriptorPoolSizeCount = 2;
									const std::array<VkDescriptorPoolSize, u32DescriptorPoolSizeCount> vk_aDescriptorPoolSizes = {
										{
											{
												.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
												.descriptorCount = RE_VK_COUNT_OF_CAMERA_UNIFORM_DESCRIPTOR * RE_VK_FRAMES_IN_FLIGHT
											}, {
												.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
												.descriptorCount = RE_VK_COUNT_OF_TEXTURE_DESCRIPTOR * RE_VK_FRAMES_IN_FLIGHT
											}
										}
									};
									if (create_descriptor_pool(u32DescriptorPoolSizeCount, vk_aDescriptorPoolSizes.data(), RE_VK_TOTAL_COUNT_OF_DESCRIPTOR_SETS, &vk_hWorldDescriptorSetPool)) {
										std::array<VkDescriptorSetLayout, RE_VK_TOTAL_COUNT_OF_DESCRIPTOR_SETS> vk_ahWorldDescriptorSetLayouts;
										vk_ahWorldDescriptorSetLayouts.fill(vk_hWorldDescriptorSetLayout);
										std::array<VkDescriptorSet, RE_VK_TOTAL_COUNT_OF_DESCRIPTOR_SETS> vk_ahAllDescriptorSets;
										if (alloc_descriptor_set(vk_hWorldDescriptorSetPool, RE_VK_TOTAL_COUNT_OF_DESCRIPTOR_SETS, vk_ahWorldDescriptorSetLayouts.data(), vk_ahAllDescriptorSets.data())) {
											std::copy(vk_ahAllDescriptorSets.begin(), vk_ahAllDescriptorSets.end(), vk_ahDescriptorSets.begin());
											if (create_pipeline_layout(1, &vk_hWorldDescriptorSetLayout, 0, nullptr, 0, &vk_hWorldBasicPipelineLayout)) {
												if (create_render_pass()) {
													constexpr uint32_t u32CameraUniformBufferQueueCount = 1;
													const std::array<uint32_t, u32CameraUniformBufferQueueCount> au32CameraUniformBufferQueues = {RE_VK_QUEUE_GRAPHICS_INDEX};
													const VkFenceCreateInfo vk_renderFenceCreateInfo = {
														.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
														.flags = VK_FENCE_CREATE_SIGNALED_BIT
													};
													uint8_t u8FrameInFlightCreateIndex = 0;
													while (u8FrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
														if (!create_vulkan_buffer(RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, u32CameraUniformBufferQueueCount, au32CameraUniformBufferQueues.data(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vk_ahWorldCameraUniformBuffers[u8FrameInFlightCreateIndex], &vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightCreateIndex])) {
															RE_FATAL_ERROR("Failed to create Vulkan uniform buffer at index ", u8FrameInFlightCreateIndex);
															break;
														}
														vkMapMemory(vk_hDevice, vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightCreateIndex], 0, RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES, 0, (void**) &apafCameraUniformData[u8FrameInFlightCreateIndex]);
														std::fill(apafCameraUniformData[u8FrameInFlightCreateIndex], apafCameraUniformData[u8FrameInFlightCreateIndex] + RE_VK_CAMERA_UNIFORM_BUFFER_SIZE_BYTES, 0.0f);
														apafCameraUniformData[u8FrameInFlightCreateIndex][RE_VK_VIEW_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(0, 0)] = 1.0f;
														apafCameraUniformData[u8FrameInFlightCreateIndex][RE_VK_VIEW_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(1, 1)] = 1.0f;
														apafCameraUniformData[u8FrameInFlightCreateIndex][RE_VK_VIEW_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(2, 2)] = 1.0f;
														apafCameraUniformData[u8FrameInFlightCreateIndex][RE_VK_VIEW_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(3, 3)] = 1.0f;
														apafCameraUniformData[u8FrameInFlightCreateIndex][RE_VK_PROJECTION_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(0, 0)] = 1.0f;
														apafCameraUniformData[u8FrameInFlightCreateIndex][RE_VK_PROJECTION_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(1, 1)] = 1.0f;
														apafCameraUniformData[u8FrameInFlightCreateIndex][RE_VK_PROJECTION_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(2, 2)] = 1.0f;
														apafCameraUniformData[u8FrameInFlightCreateIndex][RE_VK_PROJECTION_MATRIX_OFFSET + GET_VULKAN_MATRIX_ELEMENT_INDEX(3, 3)] = 1.0f;
														PUSH_TO_CALLSTACKTRACE(update_camera_descriptor_set(u8FrameInFlightCreateIndex));
														if (vkCreateFence(vk_hDevice, &vk_renderFenceCreateInfo, nullptr, &vk_ahRenderFences[u8FrameInFlightCreateIndex]) == VK_SUCCESS) {
															u8FrameInFlightCreateIndex++;
															continue;
														} else
															RE_FATAL_ERROR("Failed to create Vulkan render fence at index ", u8FrameInFlightCreateIndex);
														vkUnmapMemory(vk_hDevice, vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightCreateIndex]);
														vkFreeMemory(vk_hDevice, vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightCreateIndex], nullptr);
														vkDestroyBuffer(vk_hDevice, vk_ahWorldCameraUniformBuffers[u8FrameInFlightCreateIndex], nullptr);
														vk_ahWorldDepthStencilImageViews[u8FrameInFlightCreateIndex] = VK_NULL_HANDLE;
														vk_ahWorldDepthStencilImageMemories[u8FrameInFlightCreateIndex] = VK_NULL_HANDLE;
														vk_ahWorldDepthStencilImages[u8FrameInFlightCreateIndex] = VK_NULL_HANDLE;
														apafCameraUniformData[u8FrameInFlightCreateIndex] = nullptr;
														vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightCreateIndex] = VK_NULL_HANDLE;
														vk_ahWorldCameraUniformBuffers[u8FrameInFlightCreateIndex] = VK_NULL_HANDLE;
														break;
													}
													if (u8FrameInFlightCreateIndex == RE_VK_FRAMES_IN_FLIGHT) {
														const VkSemaphoreCreateInfo vk_renderSemaphoreCreateInfo = {
															.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
														};
														uint16_t u16RenderSemaphoreCreateIndex = 0;
														while (u16RenderSemaphoreCreateIndex < RE_VK_RENDER_SEMAPHORE_COUNT) {
															if (vkCreateSemaphore(vk_hDevice, &vk_renderSemaphoreCreateInfo, nullptr, &vk_ahRenderSemaphores[u16RenderSemaphoreCreateIndex]) != VK_SUCCESS) {
																RE_FATAL_ERROR("Failed to create Vulkan semaphore at index ", u16RenderSemaphoreCreateIndex);
																break;
															}
															u16RenderSemaphoreCreateIndex++;
														}
														if (u16RenderSemaphoreCreateIndex == RE_VK_RENDER_SEMAPHORE_COUNT && PUSH_TO_CALLSTACKTRACE_AND_RETURN(alloc_vulkan_command_buffers(vk_ahCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], VK_COMMAND_BUFFER_LEVEL_PRIMARY, RE_VK_FRAMES_IN_FLIGHT, vk_ahRenderCommandBuffers.data()), bool)) {
															const VkSubmitInfo vk_rectIndexBufferTransferSubmitInfo = {
																.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
																.commandBufferCount = 1,
																.pCommandBuffers = indexBufferDataTransferCommandBuffer.get_command_buffer_ptr()
															};
															PUSH_TO_CALLSTACKTRACE(stagingIndexBufferDataInit.join());
															if (vkQueueSubmit(vk_ahDeviceQueueFamilies[RE_VK_QUEUE_TRANSFER_INDEX], 1, &vk_rectIndexBufferTransferSubmitInfo, rectIndexBufferDataTransferFence) == VK_SUCCESS) {
																// Creates world render images on GPU, unless its size depends on swapchain image and will be created soon
																if (screenPercentageSettings.eMode != RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE || PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_world_render_images(), bool)) {
																	if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(swapchain_created_renderer(), bool)) {
																		if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(init_game_object_renderer(), bool)) {
																			fetch_minimum_allowed_texture_count();
																			rectIndexBufferDataTransferFence.wait_for();
																			return true;
																		}
																		PUSH_TO_CALLSTACKTRACE(swapchain_destroyed_renderer());
																	}
																	if (screenPercentageSettings.eMode == RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE)
																		PUSH_TO_CALLSTACKTRACE(destroy_world_render_images());
																}
																rectIndexBufferDataTransferFence.wait_for();
															} else
																RE_FATAL_ERROR("Failed to submit task to the transfer queue for copying data to the Vulkan rectangle index buffer on the GPU");
															vkFreeCommandBuffers(vk_hDevice, vk_ahCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], RE_VK_FRAMES_IN_FLIGHT, vk_ahRenderCommandBuffers.data());
															vk_ahRenderCommandBuffers.fill(VK_NULL_HANDLE);
														}
														for (uint16_t u16RenderSemaphoreDeleteIndex = 0; u16RenderSemaphoreDeleteIndex < u16RenderSemaphoreCreateIndex; u16RenderSemaphoreDeleteIndex++) {
															vkDestroySemaphore(vk_hDevice, vk_ahRenderSemaphores[u16RenderSemaphoreDeleteIndex], nullptr);
															vk_ahRenderSemaphores[u16RenderSemaphoreDeleteIndex] = VK_NULL_HANDLE;
														}
													}
													for (uint8_t u8FrameInFlightDeleteIndex = 0; u8FrameInFlightDeleteIndex < u8FrameInFlightCreateIndex; u8FrameInFlightDeleteIndex++) {
														vkUnmapMemory(vk_hDevice, vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightDeleteIndex]);
														vkFreeMemory(vk_hDevice, vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightDeleteIndex], nullptr);
														vkDestroyBuffer(vk_hDevice, vk_ahWorldCameraUniformBuffers[u8FrameInFlightDeleteIndex], nullptr);
														vkDestroyFence(vk_hDevice, vk_ahRenderFences[u8FrameInFlightDeleteIndex], nullptr);
														vk_ahWorldDepthStencilImageViews[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
														vk_ahWorldDepthStencilImageMemories[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
														vk_ahWorldDepthStencilImages[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
														apafCameraUniformData[u8FrameInFlightDeleteIndex] = nullptr;
														vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
														vk_ahWorldCameraUniformBuffers[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
														vk_ahRenderFences[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
													}
													vkDestroyRenderPass(vk_hDevice, vk_hWorldRenderPass, nullptr);
												}
												vkDestroyPipelineLayout(vk_hDevice, vk_hWorldBasicPipelineLayout, nullptr);
											} else
												RE_FATAL_ERROR("Failed to create basic Vulkan pipeline layout for rendering the world");
											vk_ahDescriptorSets.fill(VK_NULL_HANDLE);
										} else
											RE_FATAL_ERROR("Failed to allocate Vulkan descriptor set for camera uniform buffer");
										vkDestroyDescriptorPool(vk_hDevice, vk_hWorldDescriptorSetPool, nullptr);
									} else
										RE_FATAL_ERROR("Failed to create Vulkan descriptor pool for camera uniform buffer");
									vkDestroyDescriptorSetLayout(vk_hDevice, vk_hWorldDescriptorSetLayout, nullptr);
								} else
									RE_FATAL_ERROR("Failed to create Vulkan descriptor set layout for camera uniform buffer");
							} else
								RE_FATAL_ERROR("Failed to create Vulkan fence to wait on finishing transfer task for rectangle index buffer data");
						} else
							RE_FATAL_ERROR("Failed to finish recording Vulkan rectangle index buffer");
					} else
						RE_FATAL_ERROR("Failed to begin recording Vulkan rectangle index buffer");
				} else
					RE_FATAL_ERROR("Failed to allocate Vulkan command buffer to transfer data for the rectangle index buffer to the GPU");
				vkFreeMemory(vk_hDevice, vk_hRectIndexBufferMemory, nullptr);
				vkDestroyBuffer(vk_hDevice, vk_hRectIndexBuffer, nullptr);
			} else
				RE_FATAL_ERROR("Failed to create Vulkan rectangle index buffer on the GPU");
			if (stagingIndexBufferDataInit.joinable())
				stagingIndexBufferDataInit.join();
		} else
			RE_FATAL_ERROR("Failed to create Vulkan staging rectangle index buffer");
		vk_hWorldRenderPass = VK_NULL_HANDLE;
		vk_hWorldBasicPipelineLayout = VK_NULL_HANDLE;
		vk_hWorldDescriptorSetPool = VK_NULL_HANDLE;
		vk_hWorldDescriptorSetLayout = VK_NULL_HANDLE;
		vk_hRectIndexBufferMemory = VK_NULL_HANDLE;
		vk_hRectIndexBuffer = VK_NULL_HANDLE;
		return false;*/
		return true;
	}
	
	void destroy_renderer() {
		/*PUSH_TO_CALLSTACKTRACE(destroy_game_object_renderer());
		vkFreeCommandBuffers(vk_hDevice, vk_ahCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], RE_VK_FRAMES_IN_FLIGHT, vk_ahRenderCommandBuffers.data());
		for (uint16_t u16RenderSemaphoreDeleteIndex = 0; u16RenderSemaphoreDeleteIndex < RE_VK_RENDER_SEMAPHORE_COUNT; u16RenderSemaphoreDeleteIndex++) {
			vkDestroySemaphore(vk_hDevice, vk_ahRenderSemaphores[u16RenderSemaphoreDeleteIndex], nullptr);
			vk_ahRenderSemaphores[u16RenderSemaphoreDeleteIndex] = VK_NULL_HANDLE;
		}
		for (uint8_t u8FrameInFlightDeleteIndex = 0; u8FrameInFlightDeleteIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightDeleteIndex++) {
			vkDestroyImageView(vk_hDevice, vk_ahWorldDepthStencilImageViews[u8FrameInFlightDeleteIndex], nullptr);
			vkFreeMemory(vk_hDevice, vk_ahWorldDepthStencilImageMemories[u8FrameInFlightDeleteIndex], nullptr);
			vkDestroyImage(vk_hDevice, vk_ahWorldDepthStencilImages[u8FrameInFlightDeleteIndex], nullptr);
			vkUnmapMemory(vk_hDevice, vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightDeleteIndex]);
			vkFreeMemory(vk_hDevice, vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightDeleteIndex], nullptr);
			vkDestroyBuffer(vk_hDevice, vk_ahWorldCameraUniformBuffers[u8FrameInFlightDeleteIndex], nullptr);
			vkDestroyFence(vk_hDevice, vk_ahRenderFences[u8FrameInFlightDeleteIndex], nullptr);
			vk_ahWorldDepthStencilImageViews[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
			vk_ahWorldDepthStencilImageMemories[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
			vk_ahWorldDepthStencilImages[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
			apafCameraUniformData[u8FrameInFlightDeleteIndex] = nullptr;
			vk_ahWorldCameraUniformBufferMemories[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
			vk_ahWorldCameraUniformBuffers[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
			vk_ahRenderFences[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
		}
		if (screenPercentageSettings.eMode == RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE)
			PUSH_TO_CALLSTACKTRACE(destroy_world_render_images());
		if (vk_hWorldRenderPass)
			vkDestroyRenderPass(vk_hDevice, vk_hWorldRenderPass, nullptr);
		vkDestroyPipelineLayout(vk_hDevice, vk_hWorldBasicPipelineLayout, nullptr);
		vkDestroyDescriptorPool(vk_hDevice, vk_hWorldDescriptorSetPool, nullptr);
		vkDestroyDescriptorSetLayout(vk_hDevice, vk_hWorldDescriptorSetLayout, nullptr);
		vkFreeMemory(vk_hDevice, vk_hRectIndexBufferMemory, nullptr);
		vkDestroyBuffer(vk_hDevice, vk_hRectIndexBuffer, nullptr);
		vk_ahDescriptorSets.fill(VK_NULL_HANDLE);
		vk_ahRenderCommandBuffers.fill(VK_NULL_HANDLE);
		vk_hWorldRenderPass = VK_NULL_HANDLE;
		vk_hWorldBasicPipelineLayout = VK_NULL_HANDLE;
		vk_hWorldDescriptorSetPool = VK_NULL_HANDLE;
		vk_hWorldDescriptorSetLayout = VK_NULL_HANDLE;
		vk_hRectIndexBufferMemory = VK_NULL_HANDLE;
		vk_hRectIndexBuffer = VK_NULL_HANDLE;*/
	}

	void render() {
		/*vkWaitForFences(vk_hDevice, 1, &vk_ahRenderFences[u8CurrentFrameInFlightIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());
		uint32_t u32NextSwapchainImageIndex;
		const VkResult vk_eSwapchainImageAcquireResult = vkAcquireNextImageKHR(vk_hDevice, vk_hSwapchain, std::numeric_limits<uint64_t>::max(), vk_pahSwapchainSemaphores[u32NextSwapchainSemaphoreIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE], VK_NULL_HANDLE, &u32NextSwapchainImageIndex);
		switch (vk_eSwapchainImageAcquireResult) {
			case VK_SUBOPTIMAL_KHR:
			case VK_ERROR_OUT_OF_DATE_KHR:
				mark_swapchain_dirty();
				return;
			default:
				if (!check_vulkan_result(vk_eSwapchainImageAcquireResult, __FILE__, __func__, __LINE__)) {
					RE_FATAL_ERROR("Failed to get next presentable swapchain image index");
					return;
				}
				break;
		}
		reset_texture_counter();
		if (pActiveCamera && pActiveCamera->view[0] && pActiveCamera->view[1]) {
			PUSH_TO_CALLSTACKTRACE(calculate_world_render_area_with_camera());
			PUSH_TO_CALLSTACKTRACE(update_camera_uniform_buffer());
		} else {
			const VkCopyDescriptorSet vk_copyCameraDescriptorSet = {
				.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET,
				.srcSet = vk_ahDescriptorSets[u8CurrentFrameInFlightIndex == 0 ? (RE_VK_FRAMES_IN_FLIGHT - 1) : (u8CurrentFrameInFlightIndex - 1)],
				.srcBinding = 0,
				.srcArrayElement = 0,
				.dstSet = vk_ahDescriptorSets[u8CurrentFrameInFlightIndex],
				.dstBinding = 0,
				.dstArrayElement = 0,
				.descriptorCount = 1
			};
			vkUpdateDescriptorSets(vk_hDevice, 0, nullptr, 1, &vk_copyCameraDescriptorSet);
		}
		bool bNeedsRenderingGameObjects = false;
		if (!PUSH_TO_CALLSTACKTRACE_AND_RETURN(load_game_object_vertices_and_transfer(bNeedsRenderingGameObjects), bool))
			return;
		submit_sprites_to_descriptor_set(u8CurrentFrameInFlightIndex);
		if (!begin_recording_vulkan_command_buffer(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr)) {
			RE_FATAL_ERROR("Failed beginning to record Vulkan command buffer for rendering everything");
			return;
		}
		constexpr uint32_t u32ClearValueCount = 2;
		const std::array<VkClearValue, u32ClearValueCount> vk_aClearValues = {
			{
				{
					.color = {
						.float32 = {
							0.0f,
							0.0f,
							0.0f,
							1.0f
						}
					}
				}, {
					.depthStencil = {
						.depth = 0.0f,
						.stencil = 0
					}
				}
			}
		};
		const VkRenderPassBeginInfo vk_renderPassInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.renderPass = vk_hWorldRenderPass,
			.framebuffer = SHOULD_USE_SWAPCHAIN_IMAGES_DIRECTLY() ? vk_pahSwapchainFramebuffers[u32NextSwapchainImageIndex] : vk_ahWorldFramebuffers[u8CurrentFrameInFlightIndex],
			.renderArea = VkRect2D{
				VkOffset2D{0, 0},
				vk_worldRenderImageExtent2D
			},
			.clearValueCount = u32ClearValueCount,
			.pClearValues = vk_aClearValues.data()
		};
		vkCmdBeginRenderPass(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], &vk_renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		if (bNeedsRenderingGameObjects) {
			PUSH_TO_CALLSTACKTRACE(render_game_objects());
			vkCmdExecuteCommands(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], 1, &vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex]);
		}
		vkCmdEndRenderPass(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex]);
		PUSH_TO_CALLSTACKTRACE(vk_cmd_transit_image(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_ACCESS_NONE, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, SHOULD_USE_SWAPCHAIN_IMAGES_DIRECTLY() ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, vk_pahSwapchainImages[u32NextSwapchainImageIndex], VK_IMAGE_ASPECT_COLOR_BIT));
		const VkImageBlit vk_worldRenderImageBlit = {
			.srcSubresource = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
			.srcOffsets = {
				{
					.x = 0,
					.y = 0,
					.z = 0
				}, {
					.x = static_cast<int32_t>(vk_worldRenderImageExtent2D.width),
					.y = static_cast<int32_t>(vk_worldRenderImageExtent2D.height),
					.z = 1
				}
			},
			.dstSubresource = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1
			},
			.dstOffsets = {
				{
					.x = 0,
					.y = 0,
					.z = 0
				}, {
					.x = static_cast<int32_t>(vk_swapchainResolution.width),
					.y = static_cast<int32_t>(vk_swapchainResolution.height),
					.z = 1
				}
			}
		};
		if (IS_MSAA_ENABLED()) {
			const VkImageResolve vk_worldRenderImageResolve = {
				.srcSubresource = {
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.mipLevel = 0,
					.baseArrayLayer = 0,
					.layerCount = 1
				},
				.dstSubresource = {
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.mipLevel = 0,
					.baseArrayLayer = 0,
					.layerCount = 1
				},
				.extent = {
					.width = vk_worldRenderImageExtent2D.width,
					.height = vk_worldRenderImageExtent2D.height,
					.depth = 1
				}
			};
			if (IS_SCREEN_PERCENTAGE_MODIFIED()) {
				PUSH_TO_CALLSTACKTRACE(vk_cmd_transit_image(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_ACCESS_NONE, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, vk_ahSingleSampledWorldRenderImages[u8CurrentFrameInFlightIndex], VK_IMAGE_ASPECT_COLOR_BIT));
				vkCmdResolveImage(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], vk_ahWorldRenderImages[u8CurrentFrameInFlightIndex], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vk_ahSingleSampledWorldRenderImages[u8CurrentFrameInFlightIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vk_worldRenderImageResolve);
				PUSH_TO_CALLSTACKTRACE(vk_cmd_transit_image(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, vk_ahSingleSampledWorldRenderImages[u8CurrentFrameInFlightIndex], VK_IMAGE_ASPECT_COLOR_BIT));
				vkCmdBlitImage(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], vk_ahSingleSampledWorldRenderImages[u8CurrentFrameInFlightIndex], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vk_pahSwapchainImages[u32NextSwapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vk_worldRenderImageBlit, VK_FILTER_LINEAR);
			} else
				vkCmdResolveImage(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], vk_ahWorldRenderImages[u8CurrentFrameInFlightIndex], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vk_pahSwapchainImages[u32NextSwapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vk_worldRenderImageResolve);
		} else if (IS_SCREEN_PERCENTAGE_MODIFIED())
			vkCmdBlitImage(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], vk_ahWorldRenderImages[u8CurrentFrameInFlightIndex], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vk_pahSwapchainImages[u32NextSwapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vk_worldRenderImageBlit, VK_FILTER_LINEAR);
		PUSH_TO_CALLSTACKTRACE(vk_cmd_transit_image(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_NONE, SHOULD_USE_SWAPCHAIN_IMAGES_DIRECTLY() ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, vk_pahSwapchainImages[u32NextSwapchainImageIndex], VK_IMAGE_ASPECT_COLOR_BIT));
		if (vkEndCommandBuffer(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex]) != VK_SUCCESS) {
			RE_FATAL_ERROR("Failed finishing to record Vulkan command buffer for rendering everything");
			return;
		}
		vkResetFences(vk_hDevice, 1, &vk_ahRenderFences[u8CurrentFrameInFlightIndex]);
		constexpr uint32_t u32RenderSemaphoresToWaitForCount = 1 + RE_VK_SEMAPHORES_PER_FRAME_COUNT;
		const std::array<VkPipelineStageFlags, u32RenderSemaphoresToWaitForCount> vk_aePipelinesToWaitForBeforeRendering = {
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
			VK_PIPELINE_STAGE_TRANSFER_BIT
		};
		const std::array<VkSemaphore, u32RenderSemaphoresToWaitForCount> vk_ahWaitForSemaphoresBeforeRendering = {
			vk_pahSwapchainSemaphores[u32NextSwapchainSemaphoreIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE], 
			vk_ahRenderSemaphores[u8CurrentFrameInFlightIndex * RE_VK_SEMAPHORES_PER_FRAME_COUNT + RE_VK_TRANSFER_GAME_OBJECT_VERTICES_SEMAPHORE_INDEX]
		};
		if (!PUSH_TO_CALLSTACKTRACE_AND_RETURN(submit_to_vulkan_queue(vk_ahDeviceQueueFamilies[RE_VK_QUEUE_GRAPHICS_INDEX], u32RenderSemaphoresToWaitForCount, vk_ahWaitForSemaphoresBeforeRendering.data(), vk_aePipelinesToWaitForBeforeRendering.data(), 1, &vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], 1, &vk_pahSwapchainSemaphores[u32NextSwapchainSemaphoreIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE + 1], vk_ahRenderFences[u8CurrentFrameInFlightIndex]), bool)) {
			RE_FATAL_ERROR("Failed submitting the task for rendering everything to the Vulkan GPU");
			return;
		}
		const VkPresentInfoKHR vk_presentInfo = {
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &vk_pahSwapchainSemaphores[u32NextSwapchainSemaphoreIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE + 1],
			.swapchainCount = 1,
			.pSwapchains = &vk_hSwapchain,
			.pImageIndices = &u32NextSwapchainImageIndex
		};
		const VkResult vk_eSwapchainPresentResult = vkQueuePresentKHR(vk_ahDeviceQueueFamilies[RE_VK_QUEUE_PRESENT_INDEX], &vk_presentInfo);
		switch (vk_eSwapchainPresentResult) {
			case VK_SUBOPTIMAL_KHR:
			case VK_ERROR_OUT_OF_DATE_KHR:
				mark_swapchain_dirty();
				break;
			default:
				if (!check_vulkan_result(vk_eSwapchainPresentResult, __FILE__, __func__, __LINE__)) {
					RE_FATAL_ERROR("Failed to submit presentable Vulkan swapchain image");
					return;
				}
				break;
		}
		PUSH_TO_CALLSTACKTRACE(post_rendering_window_proc());
		u32NextSwapchainSemaphoreIndex = (u32NextSwapchainSemaphoreIndex + 1) % u32SwapchainImageCount;
		u8CurrentFrameInFlightIndex = (u8CurrentFrameInFlightIndex + 1) % RE_VK_FRAMES_IN_FLIGHT;*/
	}

	bool swapchain_created_renderer() {
		/*const VkSemaphoreCreateInfo vk_presentSemaphoreCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
		};
		vk_pahSwapchainSemaphores = new VkSemaphore[RE_VK_SWAPCHAIN_SEMAPHORE_COUNT];
		for (uint32_t u32PresentSemaphoreCreateIndex = 0; u32PresentSemaphoreCreateIndex < RE_VK_SWAPCHAIN_SEMAPHORE_COUNT; u32PresentSemaphoreCreateIndex++)
			if (vkCreateSemaphore(vk_hDevice, &vk_presentSemaphoreCreateInfo, nullptr, &vk_pahSwapchainSemaphores[u32PresentSemaphoreCreateIndex]) != VK_SUCCESS) {
				RE_FATAL_ERROR("Failed creating Vulkan semaphore at index ", u32PresentSemaphoreCreateIndex, " for synchronizing rendering procedures");
				for (uint32_t u8WorldRenderImageCollectionDeleteIndex = 0; u8WorldRenderImageCollectionDeleteIndex < u32PresentSemaphoreCreateIndex; u8WorldRenderImageCollectionDeleteIndex++)
					vkDestroySemaphore(vk_hDevice, vk_pahSwapchainSemaphores[u8WorldRenderImageCollectionDeleteIndex], nullptr);
				DELETE_ARRAY_SAFELY(vk_pahSwapchainSemaphores);
				return false;
			}
		if (screenPercentageSettings.eMode != RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE)
			if (!PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_world_render_images(), bool)) {
				for (uint32_t u8WorldRenderImageCollectionDeleteIndex = 0; u8WorldRenderImageCollectionDeleteIndex < RE_VK_SWAPCHAIN_SEMAPHORE_COUNT; u8WorldRenderImageCollectionDeleteIndex++)
					vkDestroySemaphore(vk_hDevice, vk_pahSwapchainSemaphores[u8WorldRenderImageCollectionDeleteIndex], nullptr);
				DELETE_ARRAY_SAFELY(vk_pahSwapchainSemaphores);
				return false;
			}
		if (!pActiveCamera) {
			vk_cameraViewport.width = vk_worldRenderImageExtent2D.width;
			vk_cameraViewport.height = vk_worldRenderImageExtent2D.height;
			vk_cameraViewport.x = 0.0f;
			vk_cameraViewport.y = 0.0f;
			vk_cameraScissor.offset.x = 0;
			vk_cameraScissor.offset.y = 0;
			vk_cameraScissor.extent.width = vk_worldRenderImageExtent2D.width;
			vk_cameraScissor.extent.height = vk_worldRenderImageExtent2D.height;
		}
		u32NextSwapchainSemaphoreIndex = 0;*/
		return true;
	}

	void swapchain_destroyed_renderer() {
		/*if (!vk_pahSwapchainSemaphores)
			return;
		for (uint32_t u8WorldRenderImageCollectionDeleteIndex = 0; u8WorldRenderImageCollectionDeleteIndex < RE_VK_SWAPCHAIN_SEMAPHORE_COUNT; u8WorldRenderImageCollectionDeleteIndex++)
			vkDestroySemaphore(vk_hDevice, vk_pahSwapchainSemaphores[u8WorldRenderImageCollectionDeleteIndex], nullptr);
		DELETE_ARRAY_SAFELY(vk_pahSwapchainSemaphores);
		if (screenPercentageSettings.eMode != RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE)
			PUSH_TO_CALLSTACKTRACE(destroy_world_render_images());*/
	}

	void attach_camera(const Camera *const pCamera) {
		pActiveCamera = pCamera;
		if (!pActiveCamera) {
			vk_cameraViewport.width = vk_worldRenderImageExtent2D.width;
			vk_cameraViewport.height = vk_worldRenderImageExtent2D.height;
			vk_cameraViewport.x = 0.0f;
			vk_cameraViewport.y = 0.0f;
			vk_cameraScissor.offset.x = 0;
			vk_cameraScissor.offset.y = 0;
			vk_cameraScissor.extent.width = vk_worldRenderImageExtent2D.width;
			vk_cameraScissor.extent.height = vk_worldRenderImageExtent2D.height;
		}
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
		PUSH_TO_CALLSTACKTRACE(destroy_world_render_images());
		screenPercentageSettings = rNewSettings;
		/*if (!bRequiresRecreatingRenderPass)
			PUSH_TO_CALLSTACKTRACE(create_world_render_images());
		else {
			vkDestroyRenderPass(vk_hDevice, vk_hWorldRenderPass, nullptr);
			if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_render_pass(), bool)) {
				if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_world_render_images(), bool)) {
					if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(recreate_game_object_render_pipelines(), bool))
						return;
					PUSH_TO_CALLSTACKTRACE(destroy_world_render_images());
				}
				vkDestroyRenderPass(vk_hDevice, vk_hWorldRenderPass, nullptr);
			}
			vk_hWorldRenderPass = VK_NULL_HANDLE;
		}*/
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
		//PUSH_TO_CALLSTACKTRACE(recreate_game_object_render_pipelines());
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
		//PUSH_TO_CALLSTACKTRACE(recreate_game_object_render_pipelines());
	}

	[[nodiscard]]
	float get_sample_shading_rate() {
		return fSampleShadingRate;
	}

}
