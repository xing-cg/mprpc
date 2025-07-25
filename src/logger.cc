#include "logger.h"
#include <time.h>
#include <thread>
#include <iostream>
Logger& Logger::GetInstance()
{
    static Logger logger;
    return logger;
}
Logger::Logger()
{
    // 启动专门的写日志线程
    std::thread writeLogTask([&](){
        for (;;)
        {
            // 获取当前的日期，推算日志文件名
            time_t now = time(nullptr);
            tm * nowtm = localtime(&now);
            char file_name[128];
            sprintf(file_name, "%d-%d-%d-log.txt", nowtm->tm_year + 1900, nowtm->tm_mon + 1, nowtm->tm_mday);
            FILE *pf = fopen(file_name, "a+");
            if (nullptr == pf)
            {
                std::cout << "logger file: " << file_name << "open error!" << std::endl;
                exit(EXIT_FAILURE);
            }
            std::string msg = m_lockQueue.Pop();
            // 时间戳
            char time_buf[128] = {0};
            sprintf(time_buf, "%d:%d:%d => ", nowtm->tm_hour, nowtm->tm_min, nowtm->tm_sec);
            msg.insert(0, time_buf);
            msg.append("\n");
            fputs(msg.c_str(), pf);
            fclose(pf);
        }
    });
    writeLogTask.detach();
}
void Logger::SetLogLevel(LogType logType)
{
    m_loglevel = logType;
}
// 外部调用。
// 把日志信息写入lockqueue中缓冲
void Logger::Log(std::string msg)
{
    m_lockQueue.Push(msg);
}
const LogType& Logger::LogLevel() const
{
    return m_loglevel;
}