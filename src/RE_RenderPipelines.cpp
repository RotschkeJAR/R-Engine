#include "RE_RenderPipelines_Internal.hpp"

namespace RE {

	bool does_gpu_support_vertex_buffers(const VkPhysicalDevice vk_hPhysicalDevice, std::queue<std::string> &rMissingFeatures) {
		VkFormatProperties2 vk_vertexBufferFormatFeatures;
		vk_vertexBufferFormatFeatures.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
		vk_vertexBufferFormatFeatures.pNext = nullptr;
		constexpr VkFormatFeatureFlags vk_eRequiredFeatures = VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT;
		constexpr VkFormat vk_aeVertexBufferFormats[] {
			VK_FORMAT_R32_UINT,
			VK_FORMAT_R32_SINT,
			VK_FORMAT_R32_SFLOAT,
			VK_FORMAT_R32G32_UINT,
			VK_FORMAT_R32G32_SINT,
			VK_FORMAT_R32G32_SFLOAT,
			VK_FORMAT_R32G32B32_UINT,
			VK_FORMAT_R32G32B32_SINT,
			VK_FORMAT_R32G32B32_SFLOAT,
			VK_FORMAT_R32G32B32A32_UINT,
			VK_FORMAT_R32G32B32A32_SINT,
			VK_FORMAT_R32G32B32A32_SFLOAT
		};
		bool bFormatsProperlySupported = true;
		for (const VkFormat vk_eVertexBufferFormat : vk_aeVertexBufferFormats) {
			vkGetPhysicalDeviceFormatProperties2(vk_hPhysicalDevice, vk_eVertexBufferFormat, &vk_vertexBufferFormatFeatures);
			if ((vk_vertexBufferFormatFeatures.formatProperties.bufferFeatures & vk_eRequiredFeatures) != vk_eRequiredFeatures) {
				bFormatsProperlySupported = false;
				rMissingFeatures.push(append_to_string("Vulkan format ", std::hex, vk_eVertexBufferFormat, " is not supported for vertex buffers on this device"));
			}
		}
		return bFormatsProperlySupported;
	}

	bool create_render_pipelines() {
		PRINT_DEBUG("Creating all Vulkan pipelines");
		return create_render_pipeline_game_objects();
	}

	bool recreate_render_pipelines() {
		PRINT_DEBUG("Recreating all Vulkan pipelines");
		return recreate_render_pipeline_game_objects();
	}

	void destroy_render_pipelines() {
		PRINT_DEBUG("Destroying all Vulkan pipelines");
		destroy_render_pipeline_game_objects();
	}

}
