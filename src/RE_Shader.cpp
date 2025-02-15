#include "RE_Shader.hpp"

#include <fstream>

namespace RE {
	
	Shader::Shader(ShaderType type, const char* fileName) : valid(false), type(type) {
		
	}
	
	Shader::Shader(ShaderType type, const uint32_t* binary) : valid(false), type(type) {
		valid = true;
	}

	Shader::~Shader() {

	}

	bool Shader::isValid() {
		return true;
	}

	Shader::operator VkShaderModule() {
		return shader;
	}

}