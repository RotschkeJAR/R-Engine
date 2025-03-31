#ifndef __RE_MAIN_H__
#define __RE_MAIN_H__

namespace RE {
	
	extern bool bRunning;
	extern bool bErrorOccured;

	extern const bool bWaylandAvailable;
	extern bool bUsingWayland;
	extern const bool bX11Available;
	extern bool bUsingX11;

	float get_deltaseconds();
	float get_fps_rate();

}

#endif /* __RE_MAIN_H__ */
