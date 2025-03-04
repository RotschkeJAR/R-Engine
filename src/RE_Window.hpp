#ifndef __RE_WINDOW_H__
#define __RE_WINDOW_H__

#include "RE_Ext Header.hpp"
#include "RE_Input.hpp"

namespace RE {
	
	class Window {
		protected:
			InputMgr inputMgr;
			const char* pcTitle;
			Vector<REushort, 2> size;
			bool bWindowVisible;
			bool bCloseFlag;
			bool bValid;

			virtual void processLoop() = 0;
			virtual void showInternal() = 0;
			virtual void updateTitleInternal() = 0;
			void updateWindowSize(REushort usNewWidth, REushort usNewHeight);

		public:
			static Window* pInstance;

			Window();
			virtual ~Window();
			void show(bool bShowWindow);
			void setTitle(const char* pNewTitle);
			void update();
			bool shouldClose();
			bool isValid();
			Vector<REushort, 2> getSize();

		friend class InputMgr;
	};

}

#endif /* __RE_WINDOW_H__ */
