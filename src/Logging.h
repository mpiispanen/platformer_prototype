#ifndef LOGGING_H
#define LOGGING_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include "Config.h"

void initializeLogging() {
    // Create a console sink
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::debug);
    console_sink->set_pattern("[%H:%M:%S] [%^%l%$] [" + std::string(PROJECT_NAME) + "] %v");

    // Create a file sink with log rotation
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/logfile.log", 1048576 * 5, 3);
    file_sink->set_level(spdlog::level::debug);
    file_sink->set_pattern("[%H:%M:%S] [%l] [" + std::string(PROJECT_NAME) + "] %v");

    // Create a logger with both sinks
    spdlog::logger logger("multi_sink", {console_sink, file_sink});
    logger.set_level(spdlog::level::debug);

    // Set the logger as the default logger
    spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger));
    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    spdlog::flush_on(spdlog::level::debug);  // Flush logs on debug level
}

#endif // LOGGING_H