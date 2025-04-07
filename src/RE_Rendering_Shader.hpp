#ifndef __RE_RENDERING_SHADER_H__
#define __RE_RENDERING_SHADER_H__

#include "RE_Vulkan.hpp"

namespace RE {
	
	class Rendering_Shader {
		private:
			VkShaderModule vk_hShader;

		public:
			Rendering_Shader() = delete;
			Rendering_Shader(const char *const pcShaderBinaryFileSource);
			~Rendering_Shader();
			VkShaderModule get_shader() const;
			bool is_valid() const;

			operator VkShaderModule() const;
	};

}

#endif /* __RE_RENDERING_SHADER_H__ */
