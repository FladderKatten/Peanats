#pragma once
#include "peanats/internal/globals.h"
#include <string>

PEANATS_SUB_NAMESPACE_BEGIN(support)

inline std::string human_readable(const std::string& source, size_t max_len = 10)
{
  std::string dest;

  for (auto& p : source)
    if (p == '\r')
      dest += "\\r";
    else if (p == '\n')
      dest += "\\n";
    else
      dest.push_back(p);

  //if (dest.size() >= max_len) {
  //  dest.resize(max_len);
  //  dest += "...";
  //}

  return dest;
}

PEANATS_SUB_NAMESPACE_END