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
			VkImage *const vk_phImage) {
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
		if (vkCreateImage(vk_hDevice, &vk_createInfo, nullptr, vk_phImage) == VK_SUCCESS)
			return true;
		else
			RE_ERROR("Failed to create Vulkan image");
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
			const VkMemoryPropertyFlags vk_mMemoryProperties) {
#ifndef RE_DISABLE_PRINT_DEBUGS
		if (valid())
			RE_ERROR("Creating another Vulkan image wrapper, when the old image ", vk_hImage, " hasn't been destroyed yet");
		if ((vk_mFlags & VK_IMAGE_CREATE_SPARSE_BINDING_BIT)) {
			RE_ERROR("A sparse-bound Vulkan image had to be created in a wrapper");
			return false;
		}
#endif
		PRINT_DEBUG_CLASS("Creating Vulkan image in wrapper class");
		if (create_vulkan_image(vk_mFlags, 
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
				&vk_hImage)) {
			PRINT_DEBUG_CLASS("Allocating memory for Vulkan image ", std::hex, vk_hImage, " in wrapper class");
			if (memory.alloc_for_image(vk_hImage, vk_mMemoryProperties) == VK_SUCCESS)
				return true;
			else
				RE_ERROR("Failed to allocate Vulkan memory for image ", std::hex, vk_hImage, " in wrapper class");
		} else
			RE_ERROR("Failed to create Vulkan image in wrapper class");
		return false;
	}

	void Vulkan_Image::destroy() {
		PRINT_DEBUG_CLASS("Destroying Vulkan image wrapper");
		vkDestroyImage(vk_hDevice, vk_hImage, nullptr);
		vk_hImage = VK_NULL_HANDLE;
		memory.free();
	}

	VkImage Vulkan_Image::get() const {
		return vk_hImage;
	}

	const VkImage* Vulkan_Image::get_ptr() const {
		return &vk_hImage;
	}

	VulkanMemory& Vulkan_Image::get_memory() {
		return memory;
	}

	bool Vulkan_Image::valid() const {
		return vk_hImage && memory.valid();
	}

	Vulkan_Image::operator VkImage() const {
		return get();
	}

	Vulkan_Image::operator const VkImage*() const {
		return get_ptr();
	}

	Vulkan_Image::operator VulkanMemory&() {
		return get_memory();
	}

	Vulkan_Image::operator bool() const {
		return valid();
	}

	VkImage Vulkan_Image::operator()() const {
		return get();
	}

}
