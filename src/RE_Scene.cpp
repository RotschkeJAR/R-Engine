#include "RE_Internal Header.hpp"
#include "RE_Manager.hpp"
#include "RE_Main.hpp"

namespace RE {
	
	Scene::Scene(const uint32_t u32Id) : u32Id(u32Id) {
		if (!u32Id)
			RE_ERROR("The scene's ID shouldn't be zero. It will be discarded if activated");
	}
	Scene::~Scene() {
		if (pCurrentScene == this) {
			RE_ERROR("The scene, which is currently in use, has been deleted");
			pCurrentScene = nullptr;
			if (pNextScene == this)
				pNextScene = nullptr;
		} else if (pNextScene == this) {
			RE_WARNING("The scene, which is dedicated to be loaded next, has been deleted");
			pNextScene = pCurrentScene;
		}
	}

	bool Scene::is_current_scene() const {
		return this == get_current_scene();
	}

	void Scene::set_next_scene() {
		RE::set_next_scene(this);
	}
	
	bool Scene::is_next_scene() const {
		return this == get_next_scene();
	}

	void Scene::start() {}
	void Scene::update() {}
	void Scene::end() {}

}
