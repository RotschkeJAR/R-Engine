#include "RE_Vulkan.hpp"
#include "RE_Render System.hpp"

namespace RE {
	
	Vulkan_DescriptorPool::Vulkan_DescriptorPool(const VkDescriptorPoolCreateFlags vk_eDescriptorPoolFlags, const uint32_t u32DescriptorPoolCount, const VkDescriptorPoolSize *vk_pDescriptorPoolSizes, const uint32_t u32MaxDescriptorSets) : vk_hDescriptorPool(VK_NULL_HANDLE) {
		VkDescriptorPoolCreateInfo vk_descriptorPoolCreateInfo = {};
		vk_descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		vk_descriptorPoolCreateInfo.flags = vk_eDescriptorPoolFlags;
		vk_descriptorPoolCreateInfo.poolSizeCount = u32DescriptorPoolCount;
		vk_descriptorPoolCreateInfo.pPoolSizes = vk_pDescriptorPoolSizes;
		vk_descriptorPoolCreateInfo.maxSets = u32MaxDescriptorSets;
		if (!CHECK_VK_RESULT(vkCreateDescriptorPool(vk_hDevice, &vk_descriptorPoolCreateInfo, nullptr, &vk_hDescriptorPool))) {
			RE_ERROR("Failed to create a Vulkan descriptor pool");
			vk_hDescriptorPool = VK_NULL_HANDLE;
		}
	}

	Vulkan_DescriptorPool::~Vulkan_DescriptorPool() {
		if (is_valid())
			CATCH_SIGNAL(vkDestroyDescriptorPool(vk_hDevice, vk_hDescriptorPool, nullptr));
	}

	VkDescriptorPool Vulkan_DescriptorPool::get_descriptor_pool() const {
		return vk_hDescriptorPool;
	}

	bool Vulkan_DescriptorPool::is_valid() const {
		return vk_hDescriptorPool != VK_NULL_HANDLE;
	}

	Vulkan_DescriptorPool::operator VkDescriptorPool() const {
		return get_descriptor_pool();
	}

}
