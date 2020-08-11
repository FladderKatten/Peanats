#pragma once
#include <peanats/internal/globals.h>
#include <peanats/peanats.h>

#if defined (_WIN32) || defined(_WIN64)
  #include "peanats/internal/platform/win.h"
#elif defined (__unix__)
  #include "peanats/internal/platform/linux.h"
#else
  #error "unsupported platfrom"
#endif
