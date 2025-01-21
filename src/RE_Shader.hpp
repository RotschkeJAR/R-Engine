#ifndef __RE_SHADER_H__
#define __RE_SHADER_H__

#include "RE_Vulkan.hpp"

namespace RE {

	class Shader {
		private:
			bool valid;
			
		public:
			Shader();
			~Shader();
			bool isValid();
	};
	
}

#endif /* __RE_SHADER_H__ */