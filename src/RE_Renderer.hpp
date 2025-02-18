#ifndef __RE_RENDERER_H__
#define __RE_RENDERER_H__

namespace RE {
	
	class Renderer {
		private:
			bool bValid;

		public:
			Renderer();
			~Renderer();
			void render();
			bool isValid();
	};

}

#endif /* __RE_RENDERER_H__ */
