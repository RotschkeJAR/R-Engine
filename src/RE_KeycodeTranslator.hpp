#ifndef __RE_KEYCODE_TRANSLATOR_H__
#define __RE_KEYCODE_TRANSLATOR_H__ 1

#include "RE_Window.hpp"

namespace RE {
	
#ifdef RE_OS_WINDOWS

	WORD virtual_win64_keycode_from_key(Input eKey);
	Input key_from_virtual_win64_keycode(WORD win_virtualKeyCode);

#elif defined RE_OS_LINUX

	KeySym virtual_x11_keycode_from_key(Input eKey);
	Input key_from_virtual_x11_keycode(KeySym x11_virtualKeyCode);

	xkb_keysym_t virtual_xkb_keysym_from_key(Input eInput);
	Input key_from_virtual_xkb_keysym(xkb_keysym_t xkb_keySym);
	
#endif

}

#endif /* __RE_KEYCODE_TRANSLATOR_H__ */
