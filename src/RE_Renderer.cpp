#include "RE_Renderer.hpp"
#include "RE_Internal Header.hpp"
#include "RE_Render System.hpp"

namespace RE {
	
	Renderer::Renderer() : primaryCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, pCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_INDEX]), bValid(false) {
		bValid = true;
	}
	Renderer::~Renderer() {}

	void Renderer::render() {
		
	}

	void Renderer::window_resize_event() {

	}

	bool Renderer::is_valid() const {
		return bValid;
	}



	SubRenderer::SubRenderer() : secondaryCommandBuffer(VK_COMMAND_BUFFER_LEVEL_SECONDARY, pCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_INDEX]), bValid(false) {}
	SubRenderer::~SubRenderer() {}

	bool SubRenderer::is_valid() const {
		return bValid;
	}

}
