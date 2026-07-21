#ifndef __RE_WINDOW_INTERNAL_H__
#define __RE_WINDOW_INTERNAL_H__

#include "RE_Window.hpp"

#include "RE_Window_Win64.hpp"
#include "RE_Window_X11.hpp"
#include "RE_Window_Wayland.hpp"

namespace RE {

#define MIN_WINDOW_WIDTH                        200
#define MIN_WINDOW_HEIGHT                       200
#define MAX_WINDOW_WIDTH_RELATIVE_TO_MONITOR    -100
#define MAX_WINDOW_HEIGHT_RELATIVE_TO_MONITOR   -100

	typedef int WindowFlags;
	enum WindowFlagBit : WindowFlags {
		WINDOW_FLAG_CREATED_BIT = 0x1,
		WINDOW_FLAG_VISIBLE_BIT = 0x2,
		WINDOW_FLAG_MINIMIZED_BIT = 0x4,
		WINDOW_FLAG_MAXIMIZED_BIT = 0x8,
		WINDOW_FLAG_CLOSE_BIT = 0x10
	};

	enum WindowArea {
		WINDOW_AREA_NONE = 0,
		WINDOW_AREA_TOP_LEFT = 1,
		WINDOW_AREA_TOP_RIGHT = 2,
		WINDOW_AREA_BOTTOM_LEFT = 3,
		WINDOW_AREA_BOTTOM_RIGHT = 4,
		WINDOW_AREA_LEFT = 5,
		WINDOW_AREA_RIGHT = 6,
		WINDOW_AREA_TOP = 7,
		WINDOW_AREA_BOTTOM = 8,
		WINDOW_AREA_BAR = 9,
		WINDOW_AREA_BUTTON_CLOSE = 10,
		WINDOW_AREA_BUTTON_MAXIMIZE = 11,
		WINDOW_AREA_BUTTON_MINIMIZE = 12,
		WINDOW_AREA_CONTENT = 13
	};
	
	extern Vector2u windowSize,
		largestMonitorSize;
	extern WindowFlags mWindowFlagBits;

	void window_resize_event(uint32_t u32NewWidth, uint32_t u32NewHeight);

}

#endif /* __RE_WINDOW_INTERNAL_H__ */
