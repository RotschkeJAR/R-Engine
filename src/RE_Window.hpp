#ifndef __RE_WINDOW_H__
#define __RE_WINDOW_H__

#include "RE_Ext Header.hpp"
#include "RE_Input.hpp"

namespace RE {
	
	class Window {
		private:
			static Window* winRef;

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

		public:
			Window();
			virtual ~Window();
			virtual void fullscreen() = 0;
			virtual void windowedFullscreen() = 0;
			virtual void window() = 0;
			void show(bool showWindow);
			void setTitle(const char* newTitle);
			void update();
			bool shouldClose();
			bool isValid();
			Vector<REushort, 2> getSize();
	};

}

#endif /* __RE_WINDOW_H__ */
