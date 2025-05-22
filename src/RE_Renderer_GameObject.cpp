#include "RE_Renderer.hpp"
#include "RE_Render System.hpp"
#include "RE_Manager.hpp"

namespace RE {

#define RE_VK_VERTEX_COUNT (RE_VK_RENDERABLE_RECTANGLES_COUNT * 4U)
#define RE_VK_VERTEX_POSITION_SIZE 3U
#define RE_VK_VERTEX_POSITION_SIZE_BYTES (RE_VK_VERTEX_POSITION_SIZE * sizeof(REgameObjectVertex_t))
#define RE_VK_VERTEX_POSITION_OFFSET 0U
#define RE_VK_VERTEX_POSITION_OFFSET_BYTES (RE_VK_VERTEX_POSITION_OFFSET * sizeof(REgameObjectVertex_t))
#define RE_VK_VERTEX_COLOR_SIZE 4U
#define RE_VK_VERTEX_COLOR_SIZE_BYTES (RE_VK_VERTEX_COLOR_SIZE * sizeof(REgameObjectVertex_t))
#define RE_VK_VERTEX_COLOR_OFFSET RE_VK_VERTEX_POSITION_SIZE
#define RE_VK_VERTEX_COLOR_OFFSET_BYTES (RE_VK_VERTEX_COLOR_OFFSET * sizeof(REgameObjectVertex_t))
#define RE_VK_VERTEX_TOTAL_SIZE (RE_VK_VERTEX_POSITION_SIZE + RE_VK_VERTEX_COLOR_SIZE)
#define RE_VK_VERTEX_TOTAL_SIZE_BYTES (RE_VK_VERTEX_TOTAL_SIZE * sizeof(REgameObjectVertex_t))
#define RE_VK_VERTEX_BUFFER_SIZE_BYTES (RE_VK_VERTEX_TOTAL_SIZE_BYTES * 4U * RE_VK_RENDERABLE_RECTANGLES_COUNT)

#define RE_VK_VIEW_MATRIX_SIZE (4U * 4U)
#define RE_VK_VIEW_MATRIX_OFFSET 0U
#define RE_VK_PROJECTION_MATRIX_SIZE (4U * 4U)
#define RE_VK_PROJECTION_MATRIX_OFFSET RE_VK_VIEW_MATRIX_SIZE
#define RE_VK_UNIFORM_BUFFER_SIZE (RE_VK_VIEW_MATRIX_SIZE + RE_VK_PROJECTION_MATRIX_SIZE)
#define RE_VK_UNIFORM_BUFFER_SIZE_BYTES (RE_VK_UNIFORM_BUFFER_SIZE * sizeof(float))

#define RE_VK_DESCRIPTOR_SET_LAYOUT_BINDING_COUNT 1U
#define RE_VK_DESCRIPTOR_POOL_SIZE 1U
#define RE_VK_VERTEX_INPUT_BINDING_DESCRIPTION_COUNT 1U
#define RE_VK_VERTEX_INPUT_ATTRIB_DESCRIPTION_COUNT 2U
#define RE_VK_VERTEX_BUFFER_QUEUE_COUNT 2U
#define RE_VK_VERTEX_STAGING_BUFFER_QUEUE_COUNT 1U
#define RE_VK_UNIFORM_BUFFER_QUEUE_COUNT 2U
#define RE_VK_UNIFORM_STAGING_BUFFER_QUEUE_COUNT 1U

	constexpr VkDescriptorSetLayoutBinding vk_descriptorSetLayoutBindings[RE_VK_DESCRIPTOR_SET_LAYOUT_BINDING_COUNT] = {
		{
			.binding = 0U,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1U,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
			.pImmutableSamplers = nullptr
		}
	};
	constexpr VkDescriptorPoolSize vk_descriptorPoolSize[RE_VK_DESCRIPTOR_POOL_SIZE] = {
		{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = RE_VK_FRAMES_IN_FLIGHT
		}
	};
	constexpr VkVertexInputBindingDescription vk_vertexInputBindingDescriptions[RE_VK_VERTEX_INPUT_BINDING_DESCRIPTION_COUNT] = {
		{
			.binding = 0U,
			.stride = RE_VK_VERTEX_TOTAL_SIZE_BYTES,
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		}
	};
	constexpr VkVertexInputAttributeDescription vk_vertexInputAttributeDescriptions[RE_VK_VERTEX_INPUT_ATTRIB_DESCRIPTION_COUNT] = {
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
	constexpr uint32_t u32vertexBuffersQueues[RE_VK_VERTEX_BUFFER_QUEUE_COUNT] = {RE_VK_QUEUE_TRANSFER_INDEX, RE_VK_QUEUE_GRAPHICS_INDEX};
	constexpr uint32_t u32vertexStagingBufferQueues[RE_VK_VERTEX_STAGING_BUFFER_QUEUE_COUNT] = {RE_VK_QUEUE_TRANSFER_INDEX};
	constexpr uint32_t u32UniformBufferQueues[RE_VK_UNIFORM_BUFFER_QUEUE_COUNT] = {RE_VK_QUEUE_TRANSFER_INDEX, RE_VK_QUEUE_GRAPHICS_INDEX};
	constexpr uint32_t u32UniformStagingBufferQueues[RE_VK_UNIFORM_STAGING_BUFFER_QUEUE_COUNT] = {RE_VK_QUEUE_TRANSFER_INDEX};
	
	Renderer_GameObject::Renderer_GameObject() 
#if (RE_VK_FRAMES_IN_FLIGHT == 2)
#else
# error Update the array initializations above!
#endif
	{
		if (!is_subrenderer_valid())
			return;
		// TODO: create static buffers
	}

	Renderer_GameObject::~Renderer_GameObject() {
		// TODO: clear memory
	}

	void Renderer_GameObject::record_secondary_command_buffer() const {
		// TODO: record secondary command buffer
	}

	void Renderer_GameObject::add_secondary_command_buffer() const {
		// TODO: add secondary command buffer to primary command buffer
	}

	void Renderer_GameObject::render() {
		if (gameObjects.empty()) {
			// TODO: skip rendering
			return;
		}
		// TODO: render game objects	
	}

	bool Renderer_GameObject::is_valid() const {
		if (!is_subrenderer_valid())
			return false;
		return true;
	}

}
