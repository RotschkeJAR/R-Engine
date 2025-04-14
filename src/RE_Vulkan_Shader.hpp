#ifndef __RE_VULKAN_SHADER_H__
#define __RE_VULKAN_SHADER_H__

#include "RE_Vulkan.hpp"

namespace RE {
	
	class Vulkan_Shader final {
		private:
			VkShaderModule vk_hShader;

		public:
			Vulkan_Shader() = delete;
			Vulkan_Shader(const char *const pcShaderBinaryFileSource);
			~Vulkan_Shader();
			VkShaderModule get_shader() const;
			bool is_valid() const;

			operator VkShaderModule() const;
	};

}

#endif /* __RE_VULKAN_SHADER_H__ */
