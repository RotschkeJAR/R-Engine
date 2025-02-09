#ifndef __RE_SHADER_H__
#define __RE_SHADER_H__

#include "RE_Vulkan.hpp"

namespace RE {

	enum ShaderType {
		Vertex,
		Geometry,
		Fragment,
		Compute
	};

	class Shader {
		private:
			bool valid;
			VkShaderModule shader;
			
		public:
			const ShaderType type;

			Shader() = delete;
			Shader(ShaderType type, const char* fileName);
			Shader(ShaderType type, const uint32_t* binary);
			~Shader();
			bool isValid();
			operator VkShaderModule();
	};

}

#endif /* __RE_SHADER_H__ */
