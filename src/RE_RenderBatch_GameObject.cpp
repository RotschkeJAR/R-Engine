#include "RE_RenderBatch_GameObject.hpp"
#include "RE_Manager.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_ListBatch_GameObject.hpp"

namespace RE {
	
	RenderBatch_GameObject::RenderBatch_GameObject(ListBatch_GameObject &rGameObjectBatch) : rGameObjectBatch(rGameObjectBatch) {
		PRINT_DEBUG_CLASS("Constructing game object render batch");
		if (vk_hDevice)
			init();
	}
	
	RenderBatch_GameObject::~RenderBatch_GameObject() {
		PRINT_DEBUG_CLASS("Destructing game object render batch");
		if (vk_hDevice)
			destroy();
	}
	
	bool RenderBatch_GameObject::init() {
		PRINT_DEBUG_CLASS("Initializing game object render batch");
		return true;
	}
	
	void RenderBatch_GameObject::destroy() {
		PRINT_DEBUG_CLASS("Destroying game object render batch");
	}
	
	void RenderBatch_GameObject::load_vertices(bool &rbNeedsRender) {
		PRINT_DEBUG_CLASS("Loading game object vertices");
	}

	void RenderBatch_GameObject::render_opaque() {
		PRINT_DEBUG_CLASS("Rendering opaque game objects");
	}

	void RenderBatch_GameObject::render_transparent() {
		PRINT_DEBUG_CLASS("Rendering transparent game objects");
	}

}
