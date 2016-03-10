#pragma once

#include <cassert>

#define CHECK(x) assert(x)
#define CHECK_MSG(x, msg) assert(x)

#define VERIFY(x) (x)

#define STATIC_ASSERT(x, msg) static_assert((x), msg)