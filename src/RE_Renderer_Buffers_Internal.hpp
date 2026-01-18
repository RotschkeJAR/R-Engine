#ifndef __RE_RENDERER_BUFFERS_INTERNAL_H__
#define __RE_RENDERER_BUFFERS_INTERNAL_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {

	extern size_t gameObjectCounterBuffer;

	bool create_object_buffers();
	void destroy_object_buffers();

	bool create_processing_buffers();
	void destroy_processing_buffers();
	
	bool create_uniform_buffers();
	void destroy_uniform_buffers();

}

#endif /* __RE_RENDERER_BUFFERS_INTERNAL_H__ */
