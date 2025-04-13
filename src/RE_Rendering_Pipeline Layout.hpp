#ifndef __RE_RENDERING_PIPELINE_LAYOUT_H__
#define __RE_RENDERING_PIPELINE_LAYOUT_H__

#include "RE_Vulkan.hpp"

namespace RE {
	
	class Rendering_PipelineLayout final {
		private:
			VkPipelineLayout vk_hPipelineLayout;

		public:
			Rendering_PipelineLayout();
			~Rendering_PipelineLayout();
			VkPipelineLayout get_pipeline_layout() const;
			bool is_valid() const;

			operator VkPipelineLayout() const;
	};

}

#endif /* __RE_RENDERING_PIPELINE_LAYOUT_H__ */