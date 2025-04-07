#include "RE_Rendering_Pipeline Layout.hpp"
#include "RE_Render System.hpp"

namespace RE {
	
	Rendering_PipelineLayout::Rendering_PipelineLayout() : vk_hPipelineLayout(VK_NULL_HANDLE) {
		VkPipelineLayoutCreateInfo vk_pipelineLayoutCreateInfo = {};
		vk_pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		vk_pipelineLayoutCreateInfo.setLayoutCount = 0U;
		vk_pipelineLayoutCreateInfo.pSetLayouts = nullptr;
		vk_pipelineLayoutCreateInfo.pushConstantRangeCount = 0U;
		vk_pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
		if (!CHECK_VK_RESULT(vkCreatePipelineLayout(vk_hDevice, &vk_pipelineLayoutCreateInfo, nullptr, &vk_hPipelineLayout)))
			vk_hPipelineLayout = VK_NULL_HANDLE;
	}

	Rendering_PipelineLayout::~Rendering_PipelineLayout() {
		if (!is_valid())
			return;
		CATCH_SIGNAL(vkDestroyPipelineLayout(vk_hDevice, vk_hPipelineLayout, nullptr));
	}

	VkPipelineLayout Rendering_PipelineLayout::get_pipeline_layout() const {
		return vk_hPipelineLayout;
	}
	
	bool Rendering_PipelineLayout::is_valid() const {
		return vk_hPipelineLayout != VK_NULL_HANDLE;
	}

	Rendering_PipelineLayout::operator VkPipelineLayout() const {
		return this->get_pipeline_layout();
	}

}
