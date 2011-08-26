#ifndef FRAMEWORK_GLOBAL_H
#define FRAMEWORK_GLOBAL_H

// common C headers
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <ctime>
#include <cmath>

// common STL headers
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <deque>
#include <stack>
#include <map>
#include <algorithm>
#include <exception>
#include <memory>
#include <type_traits>
#include <tuple>
#include <functional>
#include <typeinfo>
#include <array>
#include <iomanip>
#include <unordered_map>
#include <random>

// boost utilities
#include <boost/algorithm/string.hpp>
#include <boost/any.hpp>

// global constants
#include "const.h"

// additional utilities
#include "util/types.h"
#include "util/tools.h"
#include "util/translator.h"
#include "util/point.h"
#include "util/color.h"
#include "util/rect.h"
#include "util/size.h"

// logger
#include "core/logger.h"

// easy typing for _1, _2, ...
using namespace std::placeholders;

#endif
