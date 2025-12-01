#pragma once
#include <memory>
#include <mutex>
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

class Logger {
  public:
    static std::shared_ptr<spdlog::logger> get_instance() {
        std::call_once(init_instance_flag, &Logger::init_singleton);
        return instance;
    }

  private:
    Logger() = default;
    ~Logger() = default;
    // 初始化单例
    static void init_singleton() {
        // instance = spdlog::basic_logger_mt("file_logger",
        // "logs/peerchat.log"); 在控制台打印
        instance = spdlog::stdout_color_mt("console");
        spdlog::set_level(spdlog::level::warn);
        instance->flush_on(spdlog::level::warn);
    }

    static std::shared_ptr<spdlog::logger> instance;
    static std::once_flag init_instance_flag;
};

void logError(const std::string &message);
void logInfo(const std::string &message);
void logDebug(const std::string &message);
void logWarning(const std::string &message);