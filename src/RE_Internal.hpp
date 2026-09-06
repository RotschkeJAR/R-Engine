#ifndef __RE_INTERNAL_H__
#define __RE_INTERNAL_H__ 1

#include "RE.hpp"
#undef ERROR
#undef WARNING
#undef NOTE

#include <algorithm>
#include <chrono>
#include <fstream>
#include <list>
#include <memory>
#include <numeric>
#include <optional>
#include <queue>
#include <stack>
#include <time.h>
#include <unordered_map>
#include <vector>

#include "RE_Console.hpp"

#ifdef RE_OS_LINUX
# include <dlfcn.h>
#endif /* RE_OS_LINUX */

namespace RE {}

#endif /* __RE_INTERNAL_H__ */
