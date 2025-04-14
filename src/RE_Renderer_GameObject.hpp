#ifndef __RE_RENDERER_GAME_OBJECT_H__
#define __RE_RENDERER_GAME_OBJECT_H__

#include "RE_Renderer.hpp"
#include "RE_Vulkan_Buffer.hpp"

namespace RE {
	
	class Renderer_GameObject final : public SubRenderer {
		private:
			

		public:
			Renderer_GameObject();
			~Renderer_GameObject();
			void render();
			void window_resize_event();
	};

}

#endif /* __RE_RENDERER_GAME_OBJECT_H__ */
