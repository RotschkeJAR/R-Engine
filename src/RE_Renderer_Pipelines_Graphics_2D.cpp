#include "RE_Renderer_Pipelines_Graphics_Internal.hpp"

namespace RE {

	void setup_graphics_pipeline_2D(
			VkPipelineInputAssemblyStateCreateInfo &vk_rInputAssembly,
			VkPipelineRasterizationStateCreateInfo &vk_rRasterization,
			VkPipelineColorBlendAttachmentState &vk_rColorBlendAttachment,
			VkPipelineColorBlendStateCreateInfo &vk_rColorBlend) {
		vk_rInputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		vk_rInputAssembly.pNext = nullptr;
		vk_rInputAssembly.flags = 0;
		vk_rInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		vk_rInputAssembly.primitiveRestartEnable = VK_TRUE;
		vk_rRasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		vk_rRasterization.pNext = nullptr;
		vk_rRasterization.flags = 0;
		vk_rRasterization.depthClampEnable = VK_FALSE;
		vk_rRasterization.rasterizerDiscardEnable = VK_FALSE;
		vk_rRasterization.polygonMode = VK_POLYGON_MODE_FILL;
		vk_rRasterization.cullMode = VK_CULL_MODE_NONE;
		vk_rRasterization.depthBiasEnable = VK_FALSE;
		vk_rRasterization.depthBiasConstantFactor = 0.0f;
		vk_rRasterization.depthBiasClamp = 0.0f;
		vk_rRasterization.depthBiasSlopeFactor = 0.0f;
		vk_rRasterization.lineWidth = 1.0f;
		vk_rColorBlendAttachment.blendEnable = VK_TRUE;
		vk_rColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		vk_rColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		vk_rColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		vk_rColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		vk_rColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		vk_rColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		vk_rColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		vk_rColorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		vk_rColorBlend.pNext = nullptr;
		vk_rColorBlend.flags = 0;
		vk_rColorBlend.logicOpEnable = VK_FALSE;
		vk_rColorBlend.attachmentCount = 1;
		vk_rColorBlend.pAttachments = std::addressof(vk_rColorBlendAttachment);
		vk_rColorBlend.blendConstants[0] = 0.0f;
		vk_rColorBlend.blendConstants[1] = 0.0f;
		vk_rColorBlend.blendConstants[2] = 0.0f;
		vk_rColorBlend.blendConstants[3] = 0.0f;
	}

}
