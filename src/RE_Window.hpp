#ifndef __RE_WINDOW_H__
#define __RE_WINDOW_H__

#include "RE_Internal Header.hpp"
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

			virtual void internal_window_proc() = 0;
			virtual void internal_show_window() = 0;
			virtual void internal_update_title() = 0;
			void update_window_size(REushort usNewWidth, REushort usNewHeight);

		public:
			static Window* pInstance;

			Window();
			virtual ~Window();
			void show_window(bool bShowWindow);
			void set_window_title(const char* pNewTitle);
			void window_proc();
			bool should_close();
			bool is_valid();
			Vector<REushort, 2> get_size();

		friend class InputMgr;
	};

}

#endif /* __RE_WINDOW_H__ */
