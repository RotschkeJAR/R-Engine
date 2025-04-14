#include "RE_Vulkan_Buffer.hpp"
#include "RE_Render System.hpp"

namespace RE {
	
	Vulkan_Buffer::Vulkan_Buffer(const VkDeviceSize vk_bufferSizeInBytes, const VkBufferUsageFlags vk_bufferUsage, const VkSharingMode vk_bufferSharingMode, const uint32_t u32QueueFamilyIndexCount, const uint32_t* pu32QueueFamilyIndices, const VkMemoryPropertyFlags vk_memoryProperties) : vk_hBuffer(VK_NULL_HANDLE), vk_hBufferMemory(VK_NULL_HANDLE), vk_bufferSizeInBytes(vk_bufferSizeInBytes) {
		VkBufferCreateInfo vk_bufferCreateInfo = {};
		vk_bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vk_bufferCreateInfo.size = vk_bufferSizeInBytes;
		vk_bufferCreateInfo.usage = vk_bufferUsage;
		vk_bufferCreateInfo.sharingMode = vk_bufferSharingMode;
		switch (vk_bufferSharingMode) {
			case VK_SHARING_MODE_CONCURRENT:
				vk_bufferCreateInfo.queueFamilyIndexCount = u32QueueFamilyIndexCount;
				vk_bufferCreateInfo.pQueueFamilyIndices = pu32QueueFamilyIndices;
				break;
			case VK_SHARING_MODE_EXCLUSIVE:
			default:
				vk_bufferCreateInfo.queueFamilyIndexCount = 0U;
				vk_bufferCreateInfo.pQueueFamilyIndices = nullptr;
				break;
		}
		if (!CHECK_VK_RESULT(vkCreateBuffer(vk_hDevice, &vk_bufferCreateInfo, nullptr, &vk_hBuffer))) {
			vk_hBuffer = VK_NULL_HANDLE;
			RE_ERROR("Failed creating new buffer in Vulkan");
			return;
		}
		VkMemoryRequirements vk_bufferMemoryRequirements = {};
		CATCH_SIGNAL(vkGetBufferMemoryRequirements(vk_hDevice, vk_hBuffer, &vk_bufferMemoryRequirements));
		VkPhysicalDeviceMemoryProperties vk_physicalDeviceMemoryProperties = {};
		CATCH_SIGNAL(vkGetPhysicalDeviceMemoryProperties(vk_hPhysicalDeviceSelected, &vk_physicalDeviceMemoryProperties));
		std::optional<uint32_t> memoryTypeSelected;
		for (uint32_t u32MemoryTypeIndex = 0U; u32MemoryTypeIndex < vk_physicalDeviceMemoryProperties.memoryTypeCount; u32MemoryTypeIndex++) {
			if ((vk_bufferMemoryRequirements.memoryTypeBits & (1U << u32MemoryTypeIndex)) && (vk_physicalDeviceMemoryProperties.memoryTypes[u32MemoryTypeIndex].propertyFlags & vk_memoryProperties)) {
				memoryTypeSelected = u32MemoryTypeIndex;
				break;
			}
		}
		if (!memoryTypeSelected.has_value()) {
			RE_ERROR("The physical Vulkan device doesn't support the required memory type for the recently created buffer");
			CATCH_SIGNAL(vkDestroyBuffer(vk_hDevice, vk_hBuffer, nullptr));
			vk_hBuffer = VK_NULL_HANDLE;
			return;
		}
		VkMemoryAllocateInfo vk_bufferMemoryAllocateInfo = {};
		vk_bufferMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		vk_bufferMemoryAllocateInfo.allocationSize = vk_bufferMemoryRequirements.size;
		vk_bufferMemoryAllocateInfo.memoryTypeIndex = memoryTypeSelected.value();
		if (!CHECK_VK_RESULT(vkAllocateMemory(vk_hDevice, &vk_bufferMemoryAllocateInfo, nullptr, &vk_hBufferMemory))) {
			RE_ERROR("Failed allocating memory for the recently created buffer in Vulkan");
			vk_hBufferMemory = VK_NULL_HANDLE;
			CATCH_SIGNAL(vkDestroyBuffer(vk_hDevice, vk_hBuffer, nullptr));
			vk_hBuffer = VK_NULL_HANDLE;
			return;
		}
		CATCH_SIGNAL(vkBindBufferMemory(vk_hDevice, vk_hBuffer, vk_hBufferMemory, 0U));
	}

	Vulkan_Buffer::~Vulkan_Buffer() {
		if (!is_valid())
			return;
		CATCH_SIGNAL(vkDestroyBuffer(vk_hDevice, vk_hBuffer, nullptr));
		CATCH_SIGNAL(vkFreeMemory(vk_hDevice, vk_hBufferMemory, nullptr));
	}

	void Vulkan_Buffer::upload_data(const void *const pData, const VkDeviceSize vk_uploadSizeInBytes) const {
		void* pBufferMemory;
		CATCH_SIGNAL(vkMapMemory(vk_hDevice, vk_hBufferMemory, 0UL, vk_uploadSizeInBytes, 0, &pBufferMemory));
		CATCH_SIGNAL(std::memcpy(pBufferMemory, pData, vk_uploadSizeInBytes));
		CATCH_SIGNAL(vkUnmapMemory(vk_hDevice, vk_hBufferMemory));
	}

	VkBuffer Vulkan_Buffer::get_buffer() const {
		return vk_hBuffer;
	}
	
	VkDeviceMemory Vulkan_Buffer::get_memory() const {
		return vk_hBufferMemory;
	}

	bool Vulkan_Buffer::is_valid() const {
		return vk_hBuffer != VK_NULL_HANDLE && vk_hBufferMemory != VK_NULL_HANDLE;
	}

	Vulkan_Buffer::operator VkBuffer() const {
		return this->get_buffer();
	}
	
	Vulkan_Buffer::operator VkDeviceMemory() const {
		return this->get_memory();
	}

}
