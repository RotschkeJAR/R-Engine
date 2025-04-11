#include "RE_Renderer.hpp"
#include "RE_Internal Header.hpp"
#include "RE_Vulkan.hpp"

namespace RE {
	
	Renderer::Renderer() {
		bValid = true;
	}

	Renderer::~Renderer() {

	}

	void Renderer::prepare_render() {
		
	}

	bool Renderer::is_valid() {
		return bValid;
	}

}
