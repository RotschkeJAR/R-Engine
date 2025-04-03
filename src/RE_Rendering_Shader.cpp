#include "RE_Rendering_Shader.hpp"
#include "RE_Render System.hpp"

namespace RE {
	
	Rendering_Shader::Rendering_Shader(const char *const pcShaderBinaryFileSource) : bValid(false), vk_hShader(VK_NULL_HANDLE) {
		std::ifstream shaderBinaryFile(pcShaderBinaryFileSource, std::ios::ate | std::ios::binary);
		if (!shaderBinaryFile.is_open()) {
			RE_FATAL_ERROR(append_to_string("Failed opening the shader binary file \"", pcShaderBinaryFileSource, "\""));
			return;
		}
		const size_t shaderBinaryFileSize = shaderBinaryFile.tellg();
		char *const pcShaderBinaries = new char[shaderBinaryFileSize];
		shaderBinaryFile.seekg(0);
		CATCH_SIGNAL(shaderBinaryFile.read(pcShaderBinaries, shaderBinaryFileSize));
		shaderBinaryFile.close();
		VkShaderModuleCreateInfo vk_shaderCreateInfo = {};
		vk_shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		vk_shaderCreateInfo.codeSize = static_cast<uint32_t>(shaderBinaryFileSize);
		vk_shaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(pcShaderBinaries);
		const bool bShaderCreatedSuccessfully = CHECK_VK_RESULT(vkCreateShaderModule(get_vulkan_device(), &vk_shaderCreateInfo, nullptr, &vk_hShader));
		delete[] pcShaderBinaries;
		if (!bShaderCreatedSuccessfully) {
			RE_ERROR(append_to_string("Failed creating Vulkan shader module for the shader binaries loaded from \"", pcShaderBinaryFileSource, "\""));
			return;
		}
		bValid = true;
	}

	Rendering_Shader::~Rendering_Shader() {
		if (!bValid)
			return;
		CATCH_SIGNAL(vkDestroyShaderModule(get_vulkan_device(), vk_hShader, nullptr));
	}

	VkShaderModule Rendering_Shader::get_shader() const {
		return vk_hShader;
	}
	
	bool Rendering_Shader::is_valid() const {
		return bValid;
	}

	Rendering_Shader::operator VkShaderModule() const {
		return this->get_shader();
	}

}
