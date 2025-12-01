#include "Logger.h"

// 静态成员初始化
std::shared_ptr<spdlog::logger> Logger::instance = nullptr;
std::once_flag Logger::init_instance_flag;

void logError(const std::string &message) {
    Logger::get_instance()->error(message);
}

void logInfo(const std::string &message) {
    Logger::get_instance()->info(message);
}

void logDebug(const std::string &message) {
    Logger::get_instance()->debug(message);
}

void logWarning(const std::string &message) {
    Logger::get_instance()->warn(message);
}
