#include "RE_Renderer.hpp"
#include "RE_Ext Header.hpp"
#include "RE_Vulkan.hpp"

namespace RE {
	
	Renderer::Renderer() {
		bValid = true;
	}

	Renderer::~Renderer() {

	}

	void Renderer::render() {
		
	}

	bool Renderer::isValid() {
		return bValid;
	}

}
