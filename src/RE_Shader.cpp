#include "RE_Shader.hpp"

#include <fstream>

namespace RE {
	
	Shader::Shader(ShaderType type, const char* fileName) : valid(false), type(type) {
		std::ifstream file(fileName, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
			RE_ERROR(appendStrings("Failed opening file \"", fileName, "\" for loading shader"));
			return;
		}
		const REint sizeFile = static_cast<REint>(file.tellg());
		char* content = new char[sizeFile];
		file.seekg(0);
		file.read(content, sizeFile);
		file.close();
		valid = true;
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