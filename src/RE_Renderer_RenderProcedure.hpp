#ifndef __RE_RENDERER_RENDER_PROCEDURE_H__
#define __RE_RENDERER_RENDER_PROCEDURE_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {
	
	bool render_procedure();
#ifdef RENDERER_INCLUDE_EMPTY_PRESENT
	bool present_empty();
#endif

}

#endif /* __RE_RENDERER_RENDER_PROCEDURE_H__ */
