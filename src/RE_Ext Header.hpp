#ifndef __RE_EXT_H__
#define __RE_EXT_H__

#include "RE.hpp"
#include "RE_Console.hpp"

#ifdef RE_OS_WINDOWS
# define UNICODE
# define WIN32_LEAN_AND_MEAN /* speeds compilation up */
# include <windows.h>
#elif RE_OS_LINUX
# include <X11.h>
#endif

#include <GL/gl.h>

namespace RE {
	


}

#endif /* __RE_EXT_H__ */