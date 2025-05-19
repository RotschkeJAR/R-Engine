#include "RE_Vulkan.hpp"
#include "RE_Render System.hpp"

namespace RE {
	
	Vulkan_RenderPass::Vulkan_RenderPass(const uint32_t u32AttachmentDescriptionCount, const VkAttachmentDescription *vk_pAttachmentDescriptions, const uint32_t u32SubpassDescriptionCount, const VkSubpassDescription *vk_pSubpassDescriptions, const uint32_t u32SubpassDependencyCount, const VkSubpassDependency *vk_pSubpassDependencies) : vk_hRenderPass(VK_NULL_HANDLE) {
		VkRenderPassCreateInfo vk_renderPassCreateInfo = {};
		vk_renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		vk_renderPassCreateInfo.attachmentCount = u32AttachmentDescriptionCount;
		vk_renderPassCreateInfo.pAttachments = vk_pAttachmentDescriptions;
		vk_renderPassCreateInfo.subpassCount = u32SubpassDescriptionCount;
		vk_renderPassCreateInfo.pSubpasses = vk_pSubpassDescriptions;
		vk_renderPassCreateInfo.dependencyCount = u32SubpassDependencyCount;
		vk_renderPassCreateInfo.pDependencies = vk_pSubpassDependencies;
		if (!CHECK_VK_RESULT(vkCreateRenderPass(vk_hDevice, &vk_renderPassCreateInfo, nullptr, &vk_hRenderPass)))
			vk_hRenderPass = VK_NULL_HANDLE;
	}

	Vulkan_RenderPass::~Vulkan_RenderPass() {
		if (!is_valid())
			return;
		CATCH_SIGNAL(vkDestroyRenderPass(vk_hDevice, vk_hRenderPass, nullptr));
	}

	VkRenderPass Vulkan_RenderPass::get_render_pass() const {
		return vk_hRenderPass;
	}

	bool Vulkan_RenderPass::is_valid() const {
		return vk_hRenderPass != VK_NULL_HANDLE;
	}

	Vulkan_RenderPass::operator VkRenderPass() const {
		return this->get_render_pass();
	}

}
