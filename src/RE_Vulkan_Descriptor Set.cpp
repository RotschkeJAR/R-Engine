#include "RE_Vulkan.hpp"
#include "RE_Render System.hpp"

namespace RE {

	Vulkan_DescriptorSet::Vulkan_DescriptorSet(const VkDescriptorPool vk_hDescriptorPool) : vk_hDescriptorSet(VK_NULL_HANDLE), vk_hDescriptorPool(vk_hDescriptorPool) {}
	
	Vulkan_DescriptorSet::Vulkan_DescriptorSet(const VkDescriptorPool vk_hDescriptorPool, const VkDescriptorSetLayout vk_hDescriptorSetLayout) : vk_hDescriptorSet(VK_NULL_HANDLE), vk_hDescriptorPool(vk_hDescriptorPool) {
		VkDescriptorSetAllocateInfo vk_descriptorSetAllocInfo = {};
		vk_descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		vk_descriptorSetAllocInfo.descriptorPool = vk_hDescriptorPool;
		vk_descriptorSetAllocInfo.descriptorSetCount = 1U;
		const VkDescriptorSetLayout vk_hDescriptorSetLayoutCopy = vk_hDescriptorSetLayout;
		vk_descriptorSetAllocInfo.pSetLayouts = &vk_hDescriptorSetLayoutCopy;
		if (!CHECK_VK_RESULT(vkAllocateDescriptorSets(vk_hDevice, &vk_descriptorSetAllocInfo, &vk_hDescriptorSet))) {
			RE_ERROR("Failed to allocate a Vulkan descriptor set");
			vk_hDescriptorSet = VK_NULL_HANDLE;
		}
	}

	Vulkan_DescriptorSet::Vulkan_DescriptorSet(const Vulkan_DescriptorPool *pDescriptorPool, const Vulkan_DescriptorSetLayout *pDescriptorSetLayout) : Vulkan_DescriptorSet(CATCH_SIGNAL_AND_RETURN(pDescriptorPool->get_descriptor_pool(), VkDescriptorPool), CATCH_SIGNAL_AND_RETURN(pDescriptorSetLayout->get_descriptor_set_layout(), VkDescriptorSetLayout)) {}
	
	Vulkan_DescriptorSet::~Vulkan_DescriptorSet() {
		if (is_valid())
			CATCH_SIGNAL(vkFreeDescriptorSets(vk_hDevice, vk_hDescriptorPool, 1U, &vk_hDescriptorSet));
	}
	
	VkDescriptorSet Vulkan_DescriptorSet::get_descriptor_set() const {
		return vk_hDescriptorSet;
	}
	
	bool Vulkan_DescriptorSet::is_valid() const {
		return vk_hDescriptorSet != VK_NULL_HANDLE;
	}

	Vulkan_DescriptorSet::operator VkDescriptorSet() const {
		return get_descriptor_set();
	}



	bool alloc_vk_descriptor_sets(const VkDescriptorPool vk_hDescriptorPool, const VkDescriptorSetLayout vk_hDescriptorSetLayout, const uint32_t u32DescriptorSetCount, Vulkan_DescriptorSet **ppDescriptorSets) {
		for (uint32_t u32DescriptorSetIndex = 0U; u32DescriptorSetIndex < u32DescriptorSetCount; u32DescriptorSetIndex++)
			ppDescriptorSets[u32DescriptorSetIndex] = new Vulkan_DescriptorSet(vk_hDescriptorPool);
		VkDescriptorSetAllocateInfo vk_descriptorSetAllocInfo = {};
		vk_descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		vk_descriptorSetAllocInfo.descriptorPool = vk_hDescriptorPool;
		vk_descriptorSetAllocInfo.descriptorSetCount = u32DescriptorSetCount;
		VkDescriptorSetLayout *const vk_phDescriptorSetLayoutCopy = new VkDescriptorSetLayout[u32DescriptorSetCount];
		std::fill(vk_phDescriptorSetLayoutCopy, vk_phDescriptorSetLayoutCopy + u32DescriptorSetCount, vk_hDescriptorSetLayout);
		vk_descriptorSetAllocInfo.pSetLayouts = vk_phDescriptorSetLayoutCopy;
		VkDescriptorSet *const vk_phDescriptorSets = new VkDescriptorSet[u32DescriptorSetCount];
		if (!CHECK_VK_RESULT(vkAllocateDescriptorSets(vk_hDevice, &vk_descriptorSetAllocInfo, vk_phDescriptorSets))) {
			RE_ERROR("Failed to allocate multiple Vulkan descriptor sets at once");
			delete[] vk_phDescriptorSetLayoutCopy;
			delete[] vk_phDescriptorSets;
			return false;
		}
		for (uint32_t u32DescriptorSetIndex = 0U; u32DescriptorSetIndex < u32DescriptorSetCount; u32DescriptorSetIndex++)
			ppDescriptorSets[u32DescriptorSetIndex]->vk_hDescriptorSet = vk_phDescriptorSets[u32DescriptorSetIndex];
		delete[] vk_phDescriptorSetLayoutCopy;
		delete[] vk_phDescriptorSets;
		return true;
	}

	bool alloc_vk_descriptor_sets(const Vulkan_DescriptorPool *pDescriptorPool, const Vulkan_DescriptorSetLayout *pDescriptorSetLayout, const uint32_t u32DescriptorSetCount, Vulkan_DescriptorSet **ppDescriptorSets) {
		return CATCH_SIGNAL_AND_RETURN(alloc_vk_descriptor_sets(pDescriptorPool->get_descriptor_pool(), pDescriptorSetLayout->get_descriptor_set_layout(), u32DescriptorSetCount, ppDescriptorSets), bool);
	}
	
	void free_vk_descriptor_sets(const uint32_t u32DescriptorSetCount, Vulkan_DescriptorSet **ppDescriptorSets) {
		VkDescriptorSet *vk_phDescriptorSets = new VkDescriptorSet[u32DescriptorSetCount];
		for (uint32_t u32DescriptorSetIndex = 0U; u32DescriptorSetIndex < u32DescriptorSetCount; u32DescriptorSetIndex++) {
			vk_phDescriptorSets[u32DescriptorSetIndex] = ppDescriptorSets[u32DescriptorSetIndex]->get_descriptor_set();
			ppDescriptorSets[u32DescriptorSetIndex]->vk_hDescriptorSet = VK_NULL_HANDLE;
		}
		CATCH_SIGNAL(vkFreeDescriptorSets(vk_hDevice, ppDescriptorSets[0]->vk_hDescriptorPool, u32DescriptorSetCount, vk_phDescriptorSets));
		delete[] vk_phDescriptorSets;
		for (uint32_t u32DescriptorSetIndex = 0U; u32DescriptorSetIndex < u32DescriptorSetCount; u32DescriptorSetIndex++)
			delete ppDescriptorSets[u32DescriptorSetIndex];
	}

}
