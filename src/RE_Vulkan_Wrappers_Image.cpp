#include "RE_Vulkan_Wrappers.hpp"
#include "RE_Console.hpp"

namespace RE {
	
	bool create_vulkan_image(const VkImageCreateFlags vk_mFlags, 
			const VkImageType vk_eType, 
			const VkFormat vk_eFormat, 
			const VkExtent3D &vk_rExtent, 
			const uint32_t u32MipmapCount, 
			const uint32_t u32LayerCount, 
			const VkSampleCountFlagBits vk_eSamples, 
			const VkImageTiling vk_eTiling, 
			const VkImageUsageFlags vk_mUsage, 
			const uint32_t u32QueueFamilyCount, 
			const uint32_t *const pau32QueueFamilies, 
			const VkImageLayout vk_eInitialLayout, 
			const VulkanMemoryType eMemoryType, 
			const VkMemoryPropertyFlags vk_mMemoryProperties, 
			VkImage *const vk_phImage, 
			VulkanMemory *const pMemory) {
		PRINT_DEBUG("Creating a Vulkan image");
		VkImageCreateInfo vk_createInfo;
		vk_createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		vk_createInfo.pNext = nullptr;
		vk_createInfo.flags = vk_mFlags;
		vk_createInfo.imageType = vk_eType;
		vk_createInfo.format = vk_eFormat;
		vk_createInfo.extent = vk_rExtent;
		vk_createInfo.mipLevels = u32MipmapCount;
		vk_createInfo.arrayLayers = u32LayerCount;
		vk_createInfo.samples = vk_eSamples;
		vk_createInfo.tiling = vk_eTiling;
		vk_createInfo.usage = vk_mUsage;
		if (u32QueueFamilyCount == 1)
			vk_createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		else {
			vk_createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
			vk_createInfo.queueFamilyIndexCount = u32QueueFamilyCount;
			vk_createInfo.pQueueFamilyIndices = pau32QueueFamilies;
		}
		vk_createInfo.initialLayout = vk_eInitialLayout;
		if (vkCreateImage(vk_hDevice, &vk_createInfo, nullptr, vk_phImage) == VK_SUCCESS) {
			PRINT_DEBUG("Fetching memory requirements for Vulkan image and allocating memory for it");
			VkMemoryRequirements vk_memoryRequirements;
			vkGetImageMemoryRequirements(vk_hDevice, *vk_phImage, &vk_memoryRequirements);
			if (pMemory->alloc(vk_memoryRequirements.size, eMemoryType, vk_mMemoryProperties, vk_memoryRequirements.memoryTypeBits)) {
				PRINT_DEBUG("Binding memory and Vulkan image");
				if (vkBindImageMemory(vk_hDevice, *vk_phImage, pMemory->get(), 0) == VK_SUCCESS)
					return true;
				else
					RE_ERROR("Failed to bind memory to a Vulkan image");
				PRINT_DEBUG("Freeing memory after failing to bind memory and Vulkan image");
				pMemory->free();
			}
			PRINT_DEBUG("Destroying Vulkan image after failing to allocate memory for it");
			vkDestroyImage(vk_hDevice, *vk_phImage, nullptr);
			*vk_phImage = VK_NULL_HANDLE;
		} else
			RE_ERROR("Failed to create a Vulkan image");
		return false;
	}

	Vulkan_Image::Vulkan_Image() : vk_hImage(VK_NULL_HANDLE) {}
	
	Vulkan_Image::Vulkan_Image(const VkImageCreateFlags vk_mFlags, 
			const VkImageType vk_eType, 
			const VkFormat vk_eFormat, 
			const VkExtent3D &vk_rExtent, 
			const uint32_t u32MipmapCount, 
			const uint32_t u32LayerCount, 
			const VkSampleCountFlagBits vk_eSamples, 
			const VkImageTiling vk_eTiling, 
			const VkImageUsageFlags vk_mUsage, 
			const uint32_t u32QueueFamilyCount, 
			const uint32_t *const pau32QueueFamilies, 
			const VkImageLayout vk_eInitialLayout, 
			const VulkanMemoryType eMemoryType, 
			const VkMemoryPropertyFlags vk_mMemoryProperties) : Vulkan_Image() {
		PRINT_DEBUG_CLASS("Constructing Vulkan image wrapper");
		create(vk_mFlags, 
				vk_eType, 
				vk_eFormat, 
				vk_rExtent, 
				u32MipmapCount, 
				u32LayerCount, 
				vk_eSamples, 
				vk_eTiling, 
				vk_mUsage, 
				u32QueueFamilyCount, 
				pau32QueueFamilies, 
				vk_eInitialLayout, 
				eMemoryType, 
				vk_mMemoryProperties);
	}

	Vulkan_Image::Vulkan_Image(Vulkan_Image &&rrCopy) : vk_hImage(rrCopy.vk_hImage), memory(std::move(rrCopy.memory)) {
		PRINT_DEBUG_CLASS("Constructing Vulkan image wrapper by moving ownership from another");
		rrCopy.vk_hImage = VK_NULL_HANDLE;
	}

	Vulkan_Image::~Vulkan_Image() {
		PRINT_DEBUG_CLASS("Destructing Vulkan image wrapper");
		destroy();
	}

	bool Vulkan_Image::create(const VkImageCreateFlags vk_mFlags, 
			const VkImageType vk_eType, 
			const VkFormat vk_eFormat, 
			const VkExtent3D &vk_rExtent, 
			const uint32_t u32MipmapCount, 
			const uint32_t u32LayerCount, 
			const VkSampleCountFlagBits vk_eSamples, 
			const VkImageTiling vk_eTiling, 
			const VkImageUsageFlags vk_mUsage, 
			const uint32_t u32QueueFamilyCount, 
			const uint32_t *const pau32QueueFamilies, 
			const VkImageLayout vk_eInitialLayout, 
			const VulkanMemoryType eMemoryType, 
			const VkMemoryPropertyFlags vk_mMemoryProperties) {
#ifndef RE_DISABLE_PRINT_DEBUGS
		if (valid())
			RE_ERROR("Creating another Vulkan image wrapper, when the old image ", vk_hImage, " hasn't been destroyed yet");
#endif
		PRINT_DEBUG_CLASS("Creating Vulkan image wrapper");
		return create_vulkan_image(vk_mFlags, 
				vk_eType, 
				vk_eFormat, 
				vk_rExtent, 
				u32MipmapCount, 
				u32LayerCount, 
				vk_eSamples, 
				vk_eTiling, 
				vk_mUsage, 
				u32QueueFamilyCount, 
				pau32QueueFamilies, 
				vk_eInitialLayout, 
				eMemoryType, 
				vk_mMemoryProperties, 
				&vk_hImage, 
				&memory);
	}

	void Vulkan_Image::destroy() {
		PRINT_DEBUG_CLASS("Destroying Vulkan image wrapper");
		vkDestroyImage(vk_hDevice, vk_hImage, nullptr);
		vk_hImage = VK_NULL_HANDLE;
		memory.free();
	}

	[[nodiscard]]
	VkImage Vulkan_Image::get() const noexcept {
		return vk_hImage;
	}

	[[nodiscard]]
	const VkImage* Vulkan_Image::get_ptr() const noexcept {
		return &vk_hImage;
	}

	[[nodiscard]]
	VulkanMemory& Vulkan_Image::get_memory() noexcept {
		return memory;
	}

	[[nodiscard]]
	bool Vulkan_Image::valid() const noexcept {
		return vk_hImage && memory.valid();
	}

	[[nodiscard]]
	Vulkan_Image::operator VkImage() const {
		return get();
	}

	[[nodiscard]]
	Vulkan_Image::operator const VkImage*() const {
		return get_ptr();
	}

	[[nodiscard]]
	Vulkan_Image::operator VulkanMemory&() {
		return get_memory();
	}

	[[nodiscard]]
	Vulkan_Image::operator bool() const {
		return valid();
	}

}
