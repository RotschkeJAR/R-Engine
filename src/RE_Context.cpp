#include "RE_Context.hpp"

namespace RE {
	
	Context alloc_context() {
		return new EngineContext;
	}

	void free_context(const Context hContext) {
		delete reinterpret_cast<EngineContext*>(hContext);
	}

	EngineContext::EngineContext() {
		u8ConsoleSettings = 1 << PRINT_COLORS;

		fDeltaseconds = 0.0f;
		fMinDeltatime = 1.0f / 60;
		fMaxLagTime = DELTATIME_FOR_LOWEST_SMOOTH_FPS;
		bErrorOccured = false;
		bRunning = false;
	}

	EngineContext::~EngineContext() {}

}
