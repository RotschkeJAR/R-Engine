#ifndef __RE_WINDOW_H__
#define __RE_WINDOW_H__

#include "RE_Ext Header.hpp"
#include "RE_Input.hpp"

namespace RE {

#define CONST_WINDOW_SIZE true
	
	class Window {
		protected:
			InputMgr inputMgr;
			const char* title;
			Vector<REushort, 2> size;
			bool windowVisible;
			bool closeFlag;
			bool valid;

			virtual void processLoop() = 0;
			virtual void showInternal() = 0;
			virtual void updateTitleInternal() = 0;
			void updateWindowSize(REushort newWidth, REushort newHeight);

		public:
			static Window* instance;

			Window();
			virtual ~Window();
			void show(bool showWindow);
			void setTitle(const char* newTitle);
			void update();
			bool shouldClose();
			bool isValid();
			Vector<REushort, 2> getSize();
	};

}

#endif /* __RE_WINDOW_H__ */
