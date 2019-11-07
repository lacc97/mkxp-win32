#ifndef LOG_H
#define LOG_H

#ifndef NDEBUG
#   ifdef TRACE
#       define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#   else
#       define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#   endif
#else
#   define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif

#include <spdlog/spdlog.h>

#endif
