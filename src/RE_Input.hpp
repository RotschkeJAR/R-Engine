#ifndef __RE_INPUT_H__
#define __RE_INPUT_H__

#include "RE.hpp"

namespace RE {

#define RE_TOTAL_KEYS 0
#define RE_TOTAL_BUTTONS 0
	
	class InputMgr {
		private:
			bool keys[RE_TOTAL_KEYS], lastKeys[RE_TOTAL_KEYS];
			bool buttons[RE_TOTAL_BUTTONS], lastButtons[RE_TOTAL_BUTTONS];

		public:
			InputMgr();
			~InputMgr();
			void keyInput(REushort scancode, bool pressed);
			void charInput(wchar_t character);
			void buttonInput(REubyte buttoncode, bool pressed);
			void cursorInput(REushort x, REushort y);
			void scrollInput(float y);
	};

}

#endif
