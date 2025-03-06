#include "RE_Internal Header.hpp"
#include "RE_Manager.hpp"
#include "RE_Main.hpp"

namespace RE {
	
	Scene::Scene(REuint u32Id) : u32Id(u32Id) {
		if (!u32Id)
			RE_ERROR("The scene's ID shouldn't be zero. It will be discarded if activated");
	}
	Scene::~Scene() {
		if (Manager::pCurrentScene == this) {
			RE_ERROR("The scene, which is currently in use, has been deleted");
			Manager::pCurrentScene = nullptr;
			if (Manager::pNextScene == this)
				Manager::pNextScene = nullptr;
		} else if (Manager::pNextScene == this) {
			RE_WARNING("The scene, which is dedicated to be loaded next, has been deleted");
			Manager::pNextScene = Manager::pCurrentScene;
		}
	}

	void Scene::start() {}
	void Scene::update() {}
	void Scene::end() {}

}
