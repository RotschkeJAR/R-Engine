#include "RE_Batch_GameObject.hpp"
#include "RE_Manager.hpp"

#include <list>

namespace RE {
	
	std::list<Batch_GameObject*> gameObjectBatchList;

	Batch_GameObject::Batch_GameObject() : renderBatch(*this), u16Count(0U) {}
	Batch_GameObject::~Batch_GameObject() {}

	void Batch_GameObject::add(GameObject *const pGameObject) {
		apGameObjects[u16Count] = pGameObject;
		u16Count++;
	}
	
	bool Batch_GameObject::remove(const GameObject *const pGameObject) {
		for (uint16_t u16Index = 0U; u16Index < u16Count; u16Index++)
			if (CATCH_SIGNAL_AND_RETURN(apGameObjects[u16Index] == pGameObject, bool)) {
				u16Count--;
				CATCH_SIGNAL(apGameObjects[u16Index] = apGameObjects[u16Count]);
				return true;
			}
		return false;
	}
	
	bool Batch_GameObject::contains(const GameObject *const pGameObject) const  {
		for (uint16_t u16Index = 0U; u16Index < u16Count; u16Index++)
			if (CATCH_SIGNAL_AND_RETURN(apGameObjects[u16Index] == pGameObject, bool))
				return true;
		return false;
	}
	
	GameObject* Batch_GameObject::get(const uint16_t u16Index) {
		return apGameObjects[u16Index];
	}
	
	uint16_t Batch_GameObject::size() const {
		return u16Count;
	}
	
	bool Batch_GameObject::empty() const {
		return !u16Count;
	}

	bool Batch_GameObject::has_space() const {
		return u16Count < RE_VK_RENDERABLE_RECTANGLES_COUNT;
	}

	void Batch_GameObject::start() {
		for (uint16_t u16Index = 0U; u16Index < u16Count; u16Index++)
			CATCH_SIGNAL_DETAILED(apGameObjects[u16Index]->start(pCurrentScene), append_to_string("GameObject ", apGameObjects[u16Index], ", Index ", u16Index).c_str());
	}
	
	void Batch_GameObject::update() {
		for (uint16_t u16Index = 0U; u16Index < u16Count; u16Index++)
			CATCH_SIGNAL_DETAILED(apGameObjects[u16Index]->update(pCurrentScene), append_to_string("GameObject ", apGameObjects[u16Index], ", Index ", u16Index).c_str());
	}
	
	void Batch_GameObject::end() {
		for (uint16_t u16Index = 0U; u16Index < u16Count; u16Index++)
			CATCH_SIGNAL_DETAILED(apGameObjects[u16Index]->end(pCurrentScene), append_to_string("GameObject ", apGameObjects[u16Index], ", Index ", u16Index).c_str());
	}


	void add_to_game_object_batch(GameObject *const pGameObject) {
		size_t batchIndex = 0U;
		for (Batch_GameObject *pBatch : gameObjectBatchList) {
			if (pBatch->has_space()) {
				CATCH_SIGNAL_DETAILED(pBatch->add(pGameObject), append_to_string("Batch ", pBatch, " at index ", batchIndex).c_str());
				break;
			}
			batchIndex++;
		}
		if (batchIndex == gameObjectBatchList.size()) {
			Batch_GameObject *pNewBatch = CATCH_SIGNAL_AND_RETURN(new Batch_GameObject(), Batch_GameObject*);
			CATCH_SIGNAL(pNewBatch->add(pGameObject));
			gameObjectBatchList.push_back(pNewBatch);
		}
	}
	
	void remove_from_game_object_batch(const GameObject *const pGameObject) {
		for (Batch_GameObject *pBatch : gameObjectBatchList)
			if (CATCH_SIGNAL_AND_RETURN(pBatch->remove(pGameObject), bool)) {
				if (pBatch->empty()) {
					gameObjectBatchList.erase(std::find(gameObjectBatchList.begin(), gameObjectBatchList.end(), pBatch));
					delete pBatch;
				}
				break;
			}
	}

	void start_game_objects() {
		for (Batch_GameObject *pBatch : gameObjectBatchList)
			CATCH_SIGNAL(pBatch->start());
	}

	void update_game_objects() {
		for (Batch_GameObject *pBatch : gameObjectBatchList)
			CATCH_SIGNAL(pBatch->update());
	}

	void end_game_objects() {
		for (Batch_GameObject *pBatch : gameObjectBatchList)
			CATCH_SIGNAL(pBatch->end());
	}

	bool init_game_object_render_batches() {
		size_t batchesInitializedCount = 0U;
		for (Batch_GameObject *const pBatch : gameObjectBatchList)
			if (!CATCH_SIGNAL_AND_RETURN(pBatch->renderBatch.init(), bool))
				break;
			else
				batchesInitializedCount++;
		if (batchesInitializedCount < gameObjectBatchList.size()) {
			size_t batchesDestroyedCount = 0U;
			for (Batch_GameObject *const pBatch : gameObjectBatchList) {
				if (batchesDestroyedCount < batchesInitializedCount)
					CATCH_SIGNAL(pBatch->renderBatch.destroy());
				else
					break;
				batchesDestroyedCount++;
			}
			return false;
		}
		return true;
	}

	void destroy_game_object_render_batches() {
		for (Batch_GameObject *const pBatch : gameObjectBatchList)
			CATCH_SIGNAL(pBatch->renderBatch.destroy());
	}

	void render_opaque_game_objects() {
		for (Batch_GameObject *const pBatch : gameObjectBatchList)
			CATCH_SIGNAL(pBatch->renderBatch.render_opaque());
	}

	void render_transparent_game_objects() {
		for (Batch_GameObject *const pBatch : gameObjectBatchList)
			CATCH_SIGNAL(pBatch->renderBatch.render_transparent());
	}

}
