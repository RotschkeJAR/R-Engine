#include "RE_Renderer.hpp"
#include "RE_Render System.hpp"

namespace RE {

	typedef float REvertex_t;
#define RE_VK_VERTEX_COUNT (RE_VK_RENDERABLE_OBJECTS_COUNT * 4U)
#define RE_VK_VERTEX_POSITION_SIZE 3U
#define RE_VK_VERTEX_POSITION_SIZE_BYTES (RE_VK_VERTEX_POSITION_SIZE * sizeof(REvertex_t))
#define RE_VK_VERTEX_POSITION_OFFSET 0U
#define RE_VK_VERTEX_POSITION_OFFSET_BYTES (RE_VK_VERTEX_POSITION_OFFSET * sizeof(REvertex_t))
#define RE_VK_VERTEX_COLOR_SIZE 4U
#define RE_VK_VERTEX_COLOR_SIZE_BYTES (RE_VK_VERTEX_COLOR_SIZE * sizeof(REvertex_t))
#define RE_VK_VERTEX_COLOR_OFFSET RE_VK_VERTEX_POSITION_SIZE
#define RE_VK_VERTEX_COLOR_OFFSET_BYTES (RE_VK_VERTEX_COLOR_OFFSET * sizeof(REvertex_t))
#define RE_VK_VERTEX_TOTAL_SIZE (RE_VK_VERTEX_POSITION_SIZE + RE_VK_VERTEX_COLOR_SIZE)
#define RE_VK_VERTEX_TOTAL_SIZE_BYTES (RE_VK_VERTEX_TOTAL_SIZE * sizeof(REvertex_t))

	constexpr uint32_t u32GameObjectsVertexInputBindingDescriptionCount = 1U;
	constexpr VkVertexInputBindingDescription vk_gameObjectsVertexInputBindingDescriptions[u32GameObjectsVertexInputBindingDescriptionCount] = {
		{
			.binding = 0U,
			.stride = RE_VK_VERTEX_TOTAL_SIZE_BYTES,
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		}
	};
	constexpr uint32_t u32GameObjectsVertexInputAttributeDescriptionCount = 2U;
	constexpr VkVertexInputAttributeDescription vk_gameObjectsVertexInputAttributeDescriptions[u32GameObjectsVertexInputAttributeDescriptionCount] = {
		{
			.location = 0U,
			.binding = 0U,
			.format = VK_FORMAT_R32G32B32_SFLOAT,
			.offset = RE_VK_VERTEX_POSITION_OFFSET_BYTES
		}, {
			.location = 1U,
			.binding = 0U,
			.format = VK_FORMAT_R32G32B32A32_SFLOAT,
			.offset = RE_VK_VERTEX_COLOR_OFFSET_BYTES
		}
	};
	constexpr uint32_t u32GameObjectVertexBufferQueueCount = 1U;
	
	Renderer_GameObject::Renderer_GameObject(const Vulkan_RenderPass *pRenderPass) : gameObjectsVertexShader("shaders/vertex.spv"), 
	gameObjectsFragmentShader("shaders/fragment.spv"), 
	gameObjectsGraphicsPipeline(&gameObjectsVertexShader, &gameObjectsFragmentShader, u32GameObjectsVertexInputBindingDescriptionCount, vk_gameObjectsVertexInputBindingDescriptions, u32GameObjectsVertexInputAttributeDescriptionCount, vk_gameObjectsVertexInputAttributeDescriptions, &gameObjectsPipelineLayout, pRenderPass), 
	gameObjectVertexBuffer(RE_VK_VERTEX_TOTAL_SIZE_BYTES * 4U, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, &pDeviceQueues[RE_VK_QUEUE_GRAPHICS_INDEX]->u32QueueIndex, u32GameObjectVertexBufferQueueCount, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
		REvertex_t vertices[] = {-0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			-0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f
		};
		gameObjectVertexBuffer.upload_data(vertices, 0UL, sizeof(vertices));
	}
	Renderer_GameObject::~Renderer_GameObject() {}

	void Renderer_GameObject::record_command_buffer(const Vulkan_CommandBuffer& commandBuffer) {
		commandBuffer.cmd_bind_graphics_pipeline(&gameObjectsGraphicsPipeline);
		commandBuffer.cmd_bind_index_buffer(&Renderer::pInstance->rectangleIndexBuffer, VK_INDEX_TYPE_UINT16);
		commandBuffer.cmd_bind_vertex_buffer(&gameObjectVertexBuffer, 0UL);
		commandBuffer.cmd_set_viewport(0.0f, 0.0f, vk_swapchainResolution.width, vk_swapchainResolution.height, 0.0f, 1.0f);
		commandBuffer.cmd_set_scissor(0, 0, vk_swapchainResolution.width, vk_swapchainResolution.height);
		commandBuffer.cmd_draw_indexed(6U, 1U, 0U, 0U, 0U);
	}

	void Renderer_GameObject::record_secondary_command_buffer() {
		
	}

	void Renderer_GameObject::render() {
		
	}
	
	void Renderer_GameObject::window_resize_event() {

	}

}
