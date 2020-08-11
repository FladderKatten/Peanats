// [ Guard ]
#pragma once

// [ Dependencies ]
#include "peanats/internal/globals.h"
#include "peanats/internal/peastring.h"
#include <string>
#include <functional>

PEANATS_NAMESPACE_BEGIN

class Logger
{
  using LoggerCallback = std::function<void(const std::string&)>;
public:
  Logger(LoggerCallback cb)
    : cb(cb) {}
  
#ifdef PEANATS_DISABLE_LOGGING
    void log(const std::string& s) {}
    void logn(const std::string& s) {}
  #else
    void log(const std::string& s) { if (this && cb) cb(s); }
    void logn(const std::string& s) { if (this && cb) cb(s + "\n"); }
  #endif

private:
  LoggerCallback cb;
};

PEANATS_NAMESPACE_END