#include "RE_Internal.hpp"
#include "RE_Manager.hpp"
#include "RE_Main.hpp"

namespace RE {
	
	GameObject::GameObject(uint32_t u32OwnId, uint32_t u32SceneParentId) : bNew(true), u32OwnId(u32OwnId), u32SceneParentId(u32SceneParentId) {
		add_game_object(this);
	}
	
	GameObject::~GameObject() {
		remove_game_object(this);
	}

	void GameObject::start() {}
	void GameObject::update() {}
	void GameObject::end() {}

	Transform& GameObject::get_transform() {
		return transform;
	}
	
	SpriteRenderer& GameObject::get_sprite_renderer() {
		return spriteRenderer;
	}

}
