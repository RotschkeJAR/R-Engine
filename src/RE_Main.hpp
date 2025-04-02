#ifndef __RE_MAIN_H__
#define __RE_MAIN_H__

namespace RE {
	
	extern bool bRunning;
	extern bool bErrorOccured;
	extern WindowingSystem eUsingWindowingSystem;

	float get_deltaseconds();
	float get_fps_rate();

}

#endif /* __RE_MAIN_H__ */
