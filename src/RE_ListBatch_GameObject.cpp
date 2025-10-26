#include "RE_ListBatch_GameObject.hpp"
#include "RE_Manager.hpp"

namespace RE {

	ListBatch_GameObject::ListBatch_GameObject() : renderBatch(*this), u16Count(0) {
		PRINT_DEBUG_CLASS("Constructing list-batch for game objects");
	}
	ListBatch_GameObject::~ListBatch_GameObject() {
		PRINT_DEBUG_CLASS("Destructing list-batch for game objects");
	}

	void ListBatch_GameObject::add(GameObject *const pGameObject) {
		PRINT_DEBUG_CLASS("Adding game object ", pGameObject, " at array index ", u16Count, " and increasing count to ", u16Count + 1);
		apGameObjects[u16Count] = pGameObject;
		u16Count++;
	}
	
	bool ListBatch_GameObject::remove(const GameObject *const pGameObject) {
		PRINT_DEBUG_CLASS("Searching and removing game object ", pGameObject);
		for (uint16_t u16Index = 0; u16Index < u16Count; u16Index++)
			if (apGameObjects[u16Index] == pGameObject) {
				u16Count--;
				PRINT_DEBUG_CLASS("Removing game object at array index ", u16Index, " and decreasing count to ", u16Count);
				apGameObjects[u16Index] = apGameObjects[u16Count];
				return true;
			}
		return false;
	}
	
	bool ListBatch_GameObject::contains(const GameObject *const pGameObject) const {
		const auto endOfSearch = apGameObjects.begin() + u16Count;
		return std::find(apGameObjects.begin(), endOfSearch, pGameObject) != endOfSearch;
	}
	
	GameObject* ListBatch_GameObject::at(const uint16_t u16Index) const {
		return apGameObjects[u16Index];
	}
	
	uint16_t ListBatch_GameObject::size() const {
		return u16Count;
	}
	
	bool ListBatch_GameObject::empty() const {
		return !u16Count;
	}

	bool ListBatch_GameObject::has_space() const {
		return u16Count < apGameObjects.size();
	}

	void ListBatch_GameObject::start() {
		for (uint16_t u16Index = 0; u16Index < u16Count; u16Index++) {
			PRINT_DEBUG_CLASS("Starting game object ", apGameObjects[u16Index]);
			apGameObjects[u16Index]->start(pCurrentScene);
		}
	}
	
	void ListBatch_GameObject::update() {
		for (uint16_t u16Index = 0; u16Index < u16Count; u16Index++) {
			PRINT_DEBUG_CLASS("Updating game object ", apGameObjects[u16Index]);
			apGameObjects[u16Index]->update(pCurrentScene);
		}
	}
	
	void ListBatch_GameObject::end() {
		for (uint16_t u16Index = 0; u16Index < u16Count; u16Index++) {
			PRINT_DEBUG_CLASS("Ending game object ", apGameObjects[u16Index]);
			apGameObjects[u16Index]->end(pCurrentScene);
		}
	}

	GameObject* ListBatch_GameObject::operator[](const uint16_t u16Index) const {
		return at(u16Index);
	}

}
