#include "RE_Renderer.hpp"
#include "RE_Ext Header.hpp"
#include "vulkan/RE_Vulkan.hpp"

namespace RE {
	
	Renderer::Renderer() {
		valid = true;
	}

	Renderer::~Renderer() {

	}

	void Renderer::render() {
		
	}

	bool Renderer::isValid() {
		return valid;
	}

}
