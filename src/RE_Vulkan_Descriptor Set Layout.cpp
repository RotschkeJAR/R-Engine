#include "RE_Vulkan.hpp"
#include "RE_Render System.hpp"

namespace RE {
	
	Vulkan_DescriptorSetLayout::Vulkan_DescriptorSetLayout() : Vulkan_DescriptorSetLayout(0U, nullptr) {}

	Vulkan_DescriptorSetLayout::Vulkan_DescriptorSetLayout(const uint32_t u32BindingCount, const VkDescriptorSetLayoutBinding *vk_pDescriptorSetLayoutBindings) : vk_hDescriptorSetLayout(VK_NULL_HANDLE) {
		VkDescriptorSetLayoutCreateInfo vk_hDescriptorSetLayoutCreateInfo = {};
		vk_hDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		vk_hDescriptorSetLayoutCreateInfo.bindingCount = u32BindingCount;
		vk_hDescriptorSetLayoutCreateInfo.pBindings = vk_pDescriptorSetLayoutBindings;
		if (!CHECK_VK_RESULT(vkCreateDescriptorSetLayout(vk_hDevice, &vk_hDescriptorSetLayoutCreateInfo, nullptr, &vk_hDescriptorSetLayout))) {
			RE_ERROR("Failed to create descriptor set layout in Vulkan");
			vk_hDescriptorSetLayout = VK_NULL_HANDLE;
		}
	}

	Vulkan_DescriptorSetLayout::~Vulkan_DescriptorSetLayout() {
		if (is_valid())
			CATCH_SIGNAL(vkDestroyDescriptorSetLayout(vk_hDevice, vk_hDescriptorSetLayout, nullptr));
	}

	VkDescriptorSetLayout Vulkan_DescriptorSetLayout::get_descriptor_set_layout() const {
		return vk_hDescriptorSetLayout;
	}
	
	bool Vulkan_DescriptorSetLayout::is_valid() const {
		return vk_hDescriptorSetLayout != VK_NULL_HANDLE;
	}

	Vulkan_DescriptorSetLayout::operator VkDescriptorSetLayout() const {
		return get_descriptor_set_layout();
	}

}
