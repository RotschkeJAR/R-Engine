#include "RE_Vulkan.hpp"
#include "RE_Render System.hpp"

namespace RE {

	static VkPipelineLayout create_pipeline_layout(const uint32_t u32DescriptorSetLayoutCount, const VkDescriptorSetLayout *vk_pDescriptorSetLayouts) {
		VkPipelineLayoutCreateInfo vk_pipelineLayoutCreateInfo = {};
		vk_pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		vk_pipelineLayoutCreateInfo.setLayoutCount = u32DescriptorSetLayoutCount;
		vk_pipelineLayoutCreateInfo.pSetLayouts = vk_pDescriptorSetLayouts;
		vk_pipelineLayoutCreateInfo.pushConstantRangeCount = 0U;
		vk_pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
		VkPipelineLayout vk_hCreatedPipelineLayout;
		if (!CHECK_VK_RESULT(vkCreatePipelineLayout(vk_hDevice, &vk_pipelineLayoutCreateInfo, nullptr, &vk_hCreatedPipelineLayout))) {
			RE_ERROR("Failed to create pipeline layout in Vulkan");
			vk_hCreatedPipelineLayout = VK_NULL_HANDLE;
		}
		return vk_hCreatedPipelineLayout;
	}

	Vulkan_PipelineLayout::Vulkan_PipelineLayout() : Vulkan_PipelineLayout(0U, static_cast<VkDescriptorSetLayout*>(nullptr)) {}
	
	Vulkan_PipelineLayout::Vulkan_PipelineLayout(const uint32_t u32DescriptorSetLayoutCount, const Vulkan_DescriptorSetLayout *pDescriptorSetLayouts) : vk_hPipelineLayout(VK_NULL_HANDLE) {
		VkDescriptorSetLayout *vk_pDescriptorSetLayouts = new VkDescriptorSetLayout[u32DescriptorSetLayoutCount];
		for (uint32_t u32DescriptorSetLayoutIndex = 0U; u32DescriptorSetLayoutIndex < u32DescriptorSetLayoutCount; u32DescriptorSetLayoutIndex++)
			vk_pDescriptorSetLayouts[u32DescriptorSetLayoutIndex] = pDescriptorSetLayouts[u32DescriptorSetLayoutIndex].get_descriptor_set_layout();
		vk_hPipelineLayout = create_pipeline_layout(u32DescriptorSetLayoutCount, vk_pDescriptorSetLayouts);
		delete[] vk_pDescriptorSetLayouts;
	}
	
	Vulkan_PipelineLayout::Vulkan_PipelineLayout(const uint32_t u32DescriptorSetLayoutCount, const VkDescriptorSetLayout *vk_pDescriptorSetLayouts) : vk_hPipelineLayout(create_pipeline_layout(u32DescriptorSetLayoutCount, vk_pDescriptorSetLayouts)) {}

	Vulkan_PipelineLayout::~Vulkan_PipelineLayout() {
		if (!is_valid())
			return;
		CATCH_SIGNAL(vkDestroyPipelineLayout(vk_hDevice, vk_hPipelineLayout, nullptr));
	}

	VkPipelineLayout Vulkan_PipelineLayout::get_pipeline_layout() const {
		return vk_hPipelineLayout;
	}
	
	bool Vulkan_PipelineLayout::is_valid() const {
		return vk_hPipelineLayout != VK_NULL_HANDLE;
	}

	Vulkan_PipelineLayout::operator VkPipelineLayout() const {
		return this->get_pipeline_layout();
	}

}
