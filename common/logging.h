#ifndef COMMON_LOGGING_H
#define COMMON_LOGGING_H

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1) : __FILE__)

#ifndef SPDLOG_TRACE_ON
#define SPDLOG_TRACE_ON
#endif

#ifndef SPDLOG_DEBUG_ON
#define SPDLOG_DEBUG_ON
#endif

namespace common
{
namespace log
{
extern std::shared_ptr<spdlog::logger> m_logger;
extern bool is_save_log_file;

void initLogger();
void shutdownLogger();
void printDumpBacktrace();
void setLogLevel(std::string level);

} // namespace log
} // namespace common

#define LOG_TRACE(msg, ...) SPDLOG_LOGGER_TRACE(common::log::m_logger, msg, ##__VA_ARGS__)
#define LOG_DEBUG(msg, ...) SPDLOG_LOGGER_DEBUG(common::log::m_logger, msg, ##__VA_ARGS__)
#define LOG_INFO(msg, ...) SPDLOG_LOGGER_INFO(common::log::m_logger, msg, ##__VA_ARGS__)
#define LOG_WARN(msg, ...) SPDLOG_LOGGER_WARN(common::log::m_logger, msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) SPDLOG_LOGGER_ERROR(common::log::m_logger, msg, ##__VA_ARGS__)
#define LOG_FATAL(msg, ...) SPDLOG_LOGGER_FATAL(common::log::m_logger, msg, ##__VA_ARGS__)

#endif // COMMON_LOGGING_H
