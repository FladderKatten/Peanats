#pragma once

//#define PEANATS_DEBUG                         // Toggle extra debug output
//#define PEANATS_DISABLE_LOGGING               // Disable all logging


#define PEANATS_NAMESPACE_BEGIN        namespace peanats {
#define PEANATS_NAMESPACE_END          }
#define PEANATS_SUB_NAMESPACE_BEGIN(X) namespace peanats { namespace X {
#define PEANATS_SUB_NAMESPACE_END      }}

#ifndef  PEANATS_DEBUG
  #define PEANATS_ASSERT(COND,TEXT) if (COND) {} else {}
  #define debug_log(...)
  #define debug_logn(...)
#else
  #include <assert.h>
  #define PEANATS_ASSERT(COND,TEXT) assert((COND) && TEXT)
  #define debug_log( STRING) log( STRING)
  #define debug_logn(STRING) logn(STRING)
#endif

PEANATS_NAMESPACE_BEGIN
PEANATS_NAMESPACE_END