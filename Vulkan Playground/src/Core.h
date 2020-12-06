#pragma once

#include <memory>

// __VA_ARGS__ expansion to get past MSVC "bug"
// See https://stackoverflow.com/questions/5134523/msvc-doesnt-expand-va-args-correctly
#define EXPAND_VARGS(x) x

#ifdef DEBUG
#define ENABLE_ASSERTS
#endif

#ifdef ENABLE_ASSERTS
#define ASSERT_NO_MESSAGE(condition) { if(!(condition)) { CORE_ERROR("Assertion Failed"); __debugbreak(); } }
#define ASSERT_MESSAGE(condition, ...) { if(!(condition)) { CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }

#define ASSERT_RESOLVE(arg1, arg2, macro, ...) macro
#define GET_ASSERT_MACRO(...) EXPAND_VARGS(ASSERT_RESOLVE(__VA_ARGS__, ASSERT_MESSAGE, ASSERT_NO_MESSAGE))

#define ASSERT(...) EXPAND_VARGS( GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#else
#define ASSERT(...)
#endif
