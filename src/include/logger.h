#pragma once
#include "lockqueue.h"
enum LogType
{
    INFO, // 普通信息
    ERROR,// 错误信息
};
class Logger
{
public:
    // 单例
    static Logger& GetInstance();
    // 设置日志级别
    void SetLogLevel(LogType logType);
    // 写日志
    void Log(std::string msg);
    // 查看日志级别
    const LogType & LogLevel() const;
private:
    LogType m_loglevel;                     // 记录日志级别
    LockQueue<std::string> m_lockQueue; // 日志缓冲队列
    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
};
// 定义宏
#define LOG_INFO(logmsgformat, ...) \
    do \
    {  \
        Logger &logger = Logger::GetInstance(); \
        if (LogType::INFO < logger.LogLevel()) break; \
        std::string str("[INFO]"); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        str += c; \
        logger.Log(str); \
    } while (0)
#define LOG_ERROR(logmsgformat, ...) \
    do \
    {  \
        Logger &logger = Logger::GetInstance(); \
        if (LogType::ERROR < logger.LogLevel()) break; \
        std::string str("[ERROR]"); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        str += c; \
        logger.Log(str); \
    } while (0)