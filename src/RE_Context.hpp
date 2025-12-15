#ifndef __RE_CONTEXT_H__
#define __RE_CONTEXT_H__

namespace RE {
	
	class EngineContext final {
		public:
			// Console
#define PRINT_COLORS 0
#define TREAT_WARNING_AS_ERROR 1
#define ERRORS_ALWAYS_FATAL 2
			uint8_t u8ConsoleSettings;

			// Main
#define LOWEST_SMOOTH_FPS 20
#define DELTATIME_FOR_LOWEST_SMOOTH_FPS (1.0f / LOWEST_SMOOTH_FPS)
			float fDeltaseconds;
			float fMinDeltatime;
			float fMaxLagTime;
			bool bRunning;
			bool bErrorOccured;

			EngineContext();
			~EngineContext();
	};

#define GET_CONTEXT(CONTEXT_HANDLE) reinterpret_cast<EngineContext*>(CONTEXT_HANDLE)
#define DEFINE_PCONTEXT(CONTEXT_HANDLE) EngineContext *const pContext = GET_CONTEXT(CONTEXT_HANDLE)

}

#endif /* __RE_CONTEXT_H__ */
