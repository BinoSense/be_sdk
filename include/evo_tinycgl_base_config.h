#ifndef EVO_TINYCGL_BASE_CONFIG_H
#define EVO_TINYCGL_BASE_CONFIG_H

#if defined(_WIN32) || defined(_WIN64)
#include "platform/win/evo_tinycgl_config.h"

#elif defined(__linux__)
#if defined(__aarch64__) || defined(__arm__)
#include "platform/tegra/evo_tinycgl_config.h"
#elif defined(__x86_64__) || defined(_M_X64)
#include "platform/linux/evo_tinycgl_config.h"
#else
#error "Unsupported Linux architecture"
#endif

#else
#error "Unsupported platform"
#endif

#endif
