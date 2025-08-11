#include "RE_ListBatch_GameObject.hpp"
#include "RE_Manager.hpp"

#include <list>

namespace RE {

	ListBatch_GameObject::ListBatch_GameObject() : renderBatch(*this), u16Count(0U) {}
	ListBatch_GameObject::~ListBatch_GameObject() {}

	void ListBatch_GameObject::add(GameObject *const pGameObject) {
		apGameObjects[u16Count] = pGameObject;
		u16Count++;
	}
	
	bool ListBatch_GameObject::remove(const GameObject *const pGameObject) {
		for (uint16_t u16Index = 0; u16Index < u16Count; u16Index++)
			if (CATCH_SIGNAL_AND_RETURN(apGameObjects[u16Index] == pGameObject, bool)) {
				u16Count--;
				CATCH_SIGNAL(apGameObjects[u16Index] = apGameObjects[u16Count]);
				return true;
			}
		return false;
	}
	
	bool ListBatch_GameObject::contains(const GameObject *const pGameObject) const {
		return std::find(apGameObjects.begin(), apGameObjects.end(), pGameObject) != apGameObjects.end();
	}
	
	GameObject* ListBatch_GameObject::get(const uint16_t u16Index) {
		return apGameObjects[u16Index];
	}
	
	uint16_t ListBatch_GameObject::size() const {
		return u16Count;
	}
	
	bool ListBatch_GameObject::empty() const {
		return !u16Count;
	}

	bool ListBatch_GameObject::has_space() const {
		return u16Count < RE_VK_RENDERABLE_RECTANGLES_COUNT;
	}

	void ListBatch_GameObject::start() {
		for (uint16_t u16Index = 0; u16Index < u16Count; u16Index++)
			CATCH_SIGNAL_DETAILED(apGameObjects[u16Index]->start(pCurrentScene), append_to_string("GameObject ", apGameObjects[u16Index], ", Index ", u16Index).c_str());
	}
	
	void ListBatch_GameObject::update() {
		for (uint16_t u16Index = 0; u16Index < u16Count; u16Index++)
			CATCH_SIGNAL_DETAILED(apGameObjects[u16Index]->update(pCurrentScene), append_to_string("GameObject ", apGameObjects[u16Index], ", Index ", u16Index).c_str());
	}
	
	void ListBatch_GameObject::end() {
		for (uint16_t u16Index = 0; u16Index < u16Count; u16Index++)
			CATCH_SIGNAL_DETAILED(apGameObjects[u16Index]->end(pCurrentScene), append_to_string("GameObject ", apGameObjects[u16Index], ", Index ", u16Index).c_str());
	}

}
