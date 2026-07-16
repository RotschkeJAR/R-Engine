#ifndef __RE_MANAGER_INTERNAL_H__
#define __RE_MANAGER_INTERNAL_H__

#include "RE_Manager.hpp"

namespace RE {

	typedef uint CallingFlags_t;
	enum CallingPhase : CallingFlags_t {
		CALLING_PHASE_NONE = 0U,
		CALLING_PHASE_START = 0x1U,
		CALLING_PHASE_UPDATE = 0x2U,
		CALLING_PHASE_END = 0x3U,
		CALLING_PHASE_ADDING = 0x4U,
		CALLING_PHASE_DELETING = 0x5U
	};
	enum CallingObject : CallingFlags_t {
		CALLING_OBJECT_NONE = 0U,
		CALLING_OBJECT_SCENE = 0x1U,
		CALLING_OBJECT_GAME_OBJECT = 0x2U,
		CALLING_OBJECT_CAMERA = 0x3U
	};
#define CALLING_PHASE_BIT_OFFSET       0U
#define CALLING_OBJECT_BIT_OFFSET      3U
#define CALLING_PHASE_MASK             static_cast<CallingFlags_t>(0x3U << CALLING_PHASE_BIT_OFFSET)
#define CALLING_OBJECT_MASK            static_cast<CallingFlags_t>(0x7U << CALLING_OBJECT_BIT_OFFSET)

	extern CallingFlags_t callingPhaseObject;

#define IS_CALLING_PHASE_ACTIVE(CALLING_PHASE)             static_cast<bool>((callingPhaseObject & CALLING_PHASE_MASK) == (CALLING_PHASE << CALLING_PHASE_BIT_OFFSET))
#define IS_CALLING_OBJECT_ACTIVE(CALLING_OBJECT)           static_cast<bool>((callingPhaseObject & CALLING_OBJECT_MASK) == (CALLING_OBJECT << CALLING_OBJECT_BIT_OFFSET))
#define IS_CALLING_ACTIVE(CALLING_PHASE, CALLING_OBJECT)   (IS_CALLING_PHASE_ACTIVE(CALLING_PHASE) && IS_CALLING_OBJECT_ACTIVE(CALLING_OBJECT))
#define GET_ACTIVE_CALLING_PHASE()                         static_cast<CallingPhase>((callingPhaseObject & CALLING_PHASE_MASK) >> CALLING_PHASE_BIT_OFFSET)
#define GET_ACTIVE_CALLING_OBJECT()                        static_cast<CallingObject>((callingPhaseObject & CALLING_OBJECT_MASK) >> CALLING_OBJECT_BIT_OFFSET)
#define SET_CALLING_PHASE_ACTIVE(CALLING_PHASE)            {callingPhaseObject = (callingPhaseObject & ~CALLING_PHASE_MASK) | (CALLING_PHASE << CALLING_PHASE_BIT_OFFSET);}
#define SET_CALLING_OBJECT_ACTIVE(CALLING_OBJECT)          {callingPhaseObject = (callingPhaseObject & ~CALLING_OBJECT_MASK) | (CALLING_OBJECT << CALLING_OBJECT_BIT_OFFSET);}
#define SET_CALLING_ACTIVE(CALLING_PHASE, CALLING_OBJECT)  {SET_CALLING_PHASE_ACTIVE(CALLING_PHASE); SET_CALLING_OBJECT_ACTIVE(CALLING_OBJECT);}
#define SET_CALLING_INACTIVE()                             SET_CALLING_ACTIVE(CALLING_PHASE_NONE, CALLING_OBJECT_NONE)

#define MANAGER_CALL_PROC(TYPE, BATCH_SIZE, FIRST_BATCH, BATCH_LIST, BATCH_COUNT, MULTITHREADING_THRESHOLD, CALL_FUNC) \
		if (BATCH_COUNT <= MULTITHREADING_THRESHOLD) { \
			ListBatch<TYPE, BATCH_SIZE> *pCurrentBatch = FIRST_BATCH; \
			while (pCurrentBatch) { \
				for (uint32_t u32ListIndex = 0; u32ListIndex < pCurrentBatch->u32Count; u32ListIndex++) \
					pCurrentBatch->aList[u32ListIndex]->CALL_FUNC(); \
				pCurrentBatch = pCurrentBatch->pNext; \
			} \
		} else { \
			HardwareThreadpool threadpool; \
			std::mutex std_batchIndexMutex; \
			size_t sBatchIndex = 0; \
			for (size_t sThreadIndex = 0; sThreadIndex < decltype(threadpool)::amount_of_threads(); sThreadIndex++) { \
				threadpool.execute( \
						[&]() { \
							while (true) { \
								ListBatch<TYPE, BATCH_SIZE> *pCurrentBatch; \
								{ \
									std::lock_guard<std::mutex> std_lockGuardBatchIndex(std_batchIndexMutex); \
									if (sBatchIndex >= BATCH_COUNT) \
										return; \
									pCurrentBatch = BATCH_LIST[sBatchIndex]; \
									sBatchIndex++; \
								} \
								for (uint32_t u32ListIndex = 0; u32ListIndex < pCurrentBatch->u32Count; u32ListIndex++) \
									pCurrentBatch->aList[u32ListIndex]->CALL_FUNC(); \
							} \
						}); \
			} \
		}

	template <class T, uint32_t u32BatchSize> requires (u32BatchSize > 0)
	struct ListBatch final {
		T aList[u32BatchSize];
		ListBatch<T, u32BatchSize> *pNext;
		uint32_t u32Count;
	};
	
	bool init_manager_game_objects();
	void destroy_manager_game_objects();
	void delete_and_add_game_objects();
	void start_game_objects();
	void update_game_objects();
	void end_game_objects();

	bool init_manager_cameras();
	void destroy_manager_cameras();
	void delete_and_add_cameras();
	void start_cameras();
	void update_cameras();
	void end_cameras();

}

#endif /* __RE_MANAGER_INTERNAL_H__ */
