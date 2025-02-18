#include "RE_Ext Header.hpp"
#include "RE_Manager.hpp"

namespace RE {
	
	Scene::Scene() {}
	Scene::~Scene() {
		if (Manager::pInstance) {
			if (Manager::pInstance->pCurrentScene == this) {
				RE_ERROR("The scene, which is currently in use, has been deleted");
				Manager::pInstance->pCurrentScene = nullptr;
			} else if (Manager::pInstance->pNextScene == this) {
				RE_WARNING("The scene, which is dedicated to be loaded next, has been deleted");
				Manager::pInstance->pNextScene = nullptr;
			}
		}
	}

	void Scene::start() {}
	void Scene::update() {}
	void Scene::end() {}

}
