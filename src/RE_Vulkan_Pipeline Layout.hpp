#ifndef __RE_VULKAN_PIPELINE_LAYOUT_H__
#define __RE_VULKAN_PIPELINE_LAYOUT_H__

#include "RE_Vulkan.hpp"

namespace RE {
	
	class Vulkan_PipelineLayout final {
		private:
			VkPipelineLayout vk_hPipelineLayout;

		public:
			Vulkan_PipelineLayout();
			~Vulkan_PipelineLayout();
			VkPipelineLayout get_pipeline_layout() const;
			bool is_valid() const;

			operator VkPipelineLayout() const;
	};

}

#endif /* __RE_VULKAN_PIPELINE_LAYOUT_H__ */