#ifndef __RE_EXT_H__
#define __RE_EXT_H__

#include <algorithm>
#include <numeric>
#include <fstream>
#include <optional>
#include <vector>
#include <stack>
#include <time.h>
#include <chrono>
#include <unordered_map>
#include <list>
#include <memory>
#include <queue>
#include <functional>

#include "RE.hpp"
#undef ERROR
#undef WARNING
#undef NOTE
#include "RE_Console.hpp"

#ifdef RE_OS_LINUX
# include <dlfcn.h>
#endif /* RE_OS_LINUX */

namespace RE {}

#endif /* __RE_EXT_H__ */
