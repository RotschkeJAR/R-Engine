#include "RE_RenderBatch_GameObject.hpp"
#include "RE_Manager.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_ListBatch_GameObject.hpp"

namespace RE {
	
	RenderBatch_GameObject::RenderBatch_GameObject(ListBatch_GameObject &rGameObjectBatch) : rGameObjectBatch(rGameObjectBatch) {
		if (vk_hDevice)
			init();
	}
	
	RenderBatch_GameObject::~RenderBatch_GameObject() {
		if (vk_hDevice)
			destroy();
	}
	
	bool RenderBatch_GameObject::init() {
		return true;
	}
	
	void RenderBatch_GameObject::destroy() {
	}
	
	void RenderBatch_GameObject::load_vertices(bool &rbNeedsRender) {
	}

	void RenderBatch_GameObject::render_opaque() {
	}

	void RenderBatch_GameObject::render_transparent() {
	}

}
