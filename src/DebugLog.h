#include "config.h"

#pragma once

#include <iostream>

#ifdef DEBUG

	#define M_DEBUGTRACE(s) (std::cerr << s << std::endl)

#else //DEBUG

	#define M_DEBUGTRACE(s)

#endif //DEBUG



