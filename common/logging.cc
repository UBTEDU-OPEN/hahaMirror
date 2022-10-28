#include "logging.h"

#include <assert.h>
#include <unistd.h>

#include <memory>

#include "processinfo.h"

namespace common {
namespace log {
std::shared_ptr<spdlog::logger> m_logger;
bool is_save_log_file = true;
std::string logfile = "logs/jump.log";
std::string log_level = "debug";

static std::string toLower(std::string str) {
  int size = str.size();
  for (int i = 0; i < size; ++i) {
    str[i] = tolower(str[i]);
  }

  return str;
}

static spdlog::level::level_enum getLogLevel() {
  using namespace spdlog::level;

  if (log_level == "trace") {
    return trace;
  } else if (log_level == "debug") {
    return debug;
  } else if (log_level == "info") {
    return info;
  } else if (log_level == "warn") {
    return warn;
  } else if (log_level == "err") {
    return err;
  } else if (log_level == "critical") {
    return critical;
  }

  return debug;
}

void setLogLevel(std::string level) { log_level = toLower(level); }

void initLogger() {
  if (spdlog::get("console") != nullptr) {
    m_logger = spdlog::get("console");
    return;
  }

  logfile = common::ProcessInfo::exeDirPath() + "/" + logfile;

  std::vector<spdlog::sink_ptr> sinkList;
  auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  consoleSink->set_level(getLogLevel());
  sinkList.push_back(consoleSink);
  printf(
      "+++++++++++++++++++++++++++++ log_level: "
      "%s, %d++++++++++++++++++++++++++++\n",
      log_level.c_str(), getLogLevel());
  if (is_save_log_file) {
    spdlog::sink_ptr rotateSink = nullptr;
    rotateSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        logfile, 1024 * 1024 * 10, 3);
    rotateSink->set_level(getLogLevel());
    sinkList.push_back(rotateSink);
  }
  m_logger = std::make_shared<spdlog::logger>("multi_sink", begin(sinkList),
                                              end(sinkList));
  m_logger->set_level(getLogLevel());
  m_logger->flush_on(spdlog::level::debug);
  m_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%^%l%$][%s:%#, %!] %v");
}

void shutdownLogger() {
  spdlog::drop_all();
  spdlog::shutdown();
}

void printDumpBacktrace() { m_logger->dump_backtrace(); }

}  // namespace log
}  // namespace common
