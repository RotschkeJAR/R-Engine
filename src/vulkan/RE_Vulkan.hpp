#ifndef __RE_VK_H__
#define __RE_VK_H__

#include "../RE_Ext Header.hpp"
#include "RE_Vulkan Core.hpp"

namespace RE {

	class Vulkan {
		private:
			REubyte validation;
			VulkanCore* core;

		public:
			Vulkan();
			~Vulkan();
			bool isValid();
	};

}

#endif /* __RE_VK_H__ */
