#include "RE_Internal Header.hpp"
#include "RE_Manager.hpp"
#include "RE_Main.hpp"
#include "RE_Batch_GameObject.hpp"

namespace RE {
	
	GameObject::GameObject(const uint32_t u32OwnId, const uint32_t u32SceneParentId) : u32OwnId(u32OwnId), u32SceneParentId(u32SceneParentId) {
		newGameObjects.push_back(this);
	}
	GameObject::~GameObject() {
		CATCH_SIGNAL(remove_from_game_object_batch(this));
	}

	void GameObject::mark_deletable() {
		deletableGameObjects.push_back(this);
	}

	void GameObject::start(Scene *const pStartingScene) {}
	void GameObject::update(Scene *const pCurrentScene) {}
	void GameObject::end(Scene *const pEndingScene) {}

}
