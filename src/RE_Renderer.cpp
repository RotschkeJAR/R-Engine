#include "RE_Renderer.hpp"
#include "RE_Internal Header.hpp"
#include "RE_Vulkan.hpp"

namespace RE {
	
	Renderer::Renderer() : bValid(false) {}
	Renderer::~Renderer() {}

	void Renderer::render() {

	}

	bool Renderer::is_valid() const {
		return bValid;
	}



	SubRenderer::SubRenderer() : bValid(false) {}
	SubRenderer::~SubRenderer() {}

	void SubRenderer::render() {

	}

	bool SubRenderer::is_valid() const {
		return bValid;
	}

}
