#include "RE_Vulkan_Wrappers.hpp"
#include "RE_Console.hpp"

namespace RE {
	
	bool create_vulkan_shader_from_file(const char *const pacDirectory, const VkShaderModuleCreateFlags vk_eFlags, VkShaderModule *const vk_phShaderModule) {
		PRINT_DEBUG("Opening shader binary file \"", pacDirectory, "\"");
		std::ifstream shaderFile(pacDirectory, std::ios::ate | std::ios::binary);
		if (shaderFile.is_open()) {
			PRINT_DEBUG("Reading contents from file and creating Vulkan shader module");
			size_t shaderBinarySize = shaderFile.tellg();
			std::vector<char> shaderBinaries;
			shaderBinaries.resize(shaderBinarySize);
			shaderFile.seekg(0);
			shaderFile.read(shaderBinaries.data(), shaderBinarySize);
			shaderFile.close();
			const VkShaderModuleCreateInfo vk_createInfo = {
				.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
				.flags = vk_eFlags,
				.codeSize = static_cast<uint32_t>(shaderBinarySize),
				.pCode = reinterpret_cast<uint32_t*>(shaderBinaries.data())
			};
			if (vkCreateShaderModule(vk_hDevice, &vk_createInfo, nullptr, vk_phShaderModule) == VK_SUCCESS)
				return true;
			else
				RE_ERROR("Failed to create a Vulkan shader module");
		} else
			RE_ERROR("Failed to open shader binary file \"", pacDirectory, "\"");
		return false;
	}

	Vulkan_Shader::Vulkan_Shader() : vk_hShaderModule(VK_NULL_HANDLE) {}

	Vulkan_Shader::Vulkan_Shader(const char *const pacDirectory, const VkShaderModuleCreateFlags vk_eFlags) : vk_hShaderModule(VK_NULL_HANDLE) {
		PRINT_DEBUG_CLASS("Constructing Vulkan shader module wrapper");
		create(pacDirectory, vk_eFlags);
	}

	Vulkan_Shader::Vulkan_Shader(Vulkan_Shader &&rrCopy) : vk_hShaderModule(rrCopy.vk_hShaderModule) {
		PRINT_DEBUG_CLASS("Constructing Vulkan shader module wrapper by moving ownership from another wrapper");
		rrCopy.vk_hShaderModule = VK_NULL_HANDLE;
	}

	Vulkan_Shader::~Vulkan_Shader() {
		PRINT_DEBUG_CLASS("Deconstructing Vulkan shader module wrapper");
		destroy();
	}

	bool Vulkan_Shader::create(const char *const pacDirectory, const VkShaderModuleCreateFlags vk_eFlags) {
#ifndef RE_DISABLE_PRINT_DEBUGS
		if (valid())
			RE_ERROR("Creating another Vulkan shader module wrapper, when the old shader module ", vk_hShaderModule, " hasn't been destroyed yet");
#endif
		PRINT_DEBUG_CLASS("Creating Vulkan shader module wrapper");
		return create_vulkan_shader_from_file(pacDirectory, vk_eFlags, &vk_hShaderModule);
	}

	void Vulkan_Shader::destroy() {
		PRINT_DEBUG_CLASS("Destroying Vulkan shader module wrapper");
		vkDestroyShaderModule(vk_hDevice, vk_hShaderModule, nullptr);
		vk_hShaderModule = VK_NULL_HANDLE;
	}

	VkShaderModule Vulkan_Shader::get() const {
		return vk_hShaderModule;
	}

	const VkShaderModule* Vulkan_Shader::get_ptr() const {
		return &vk_hShaderModule;
	}

	bool Vulkan_Shader::valid() const {
		return vk_hShaderModule != VK_NULL_HANDLE;
	}

	Vulkan_Shader::operator VkShaderModule() const {
		return get();
	}

	Vulkan_Shader::operator const VkShaderModule*() const {
		return get_ptr();
	}

	Vulkan_Shader::operator bool() const {
		return valid();
	}

	VkShaderModule Vulkan_Shader::operator()() const {
		return static_cast<VkShaderModule>(*this);
	}

}
