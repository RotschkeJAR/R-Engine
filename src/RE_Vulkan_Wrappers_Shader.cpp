#include "RE_Vulkan_Wrappers.hpp"
#include "RE_Console.hpp"

namespace RE {
	
	bool create_vulkan_shader_from_file(const char *const pacDirectory, const VkShaderModuleCreateFlags vk_eFlags, VkShaderModule *const vk_phShaderModule) {
		std::ifstream shaderFile(pacDirectory, std::ios::ate | std::ios::binary);
		if (shaderFile.is_open()) {
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
			return vkCreateShaderModule(vk_hDevice, &vk_createInfo, nullptr, vk_phShaderModule) == VK_SUCCESS;
		}
		return false;
	}

}
