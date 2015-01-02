#include "logger.hpp"
#include <ctime>
#include <sys/time.h>

std::unique_ptr<AbstractLogger> logger(new Logger());

void SetLogger(AbstractLogger* logobj)
{
    logger.reset(logobj);
}

inline const char* GetLogAbbrev(LogLevel level)
{
    switch(level)
    {
        case LogLevel::FATAL:
            return "F";
        case LogLevel::DEBUG:
            return "D";
        case LogLevel::WARNING:
            return "W";
        case LogLevel::INFO:
            return "I";
        case LogLevel::VERBOSE:
            return "V";
        default:
            return "?";
    }
}

inline const char* GetLogColor(LogLevel level)
{
    switch(level)
    {
        case LogLevel::FATAL:
            return "\033[1;31m";
        case LogLevel::DEBUG:
            return "\033[1;33m";
        case LogLevel::WARNING:
            return "\033[1;33m";
        case LogLevel::INFO:
            return "";
        case LogLevel::VERBOSE:
            return "\033[2;34m";
        default:
            return "";
    }
}

void AbstractLogger::Log(LogLevel level, const char* format, va_list ap)
{
    if (level > log_level)
        return;

    if (log_time)
    {
        timeval curTime;
        gettimeofday(&curTime, NULL);
        char buffer[128];
        strftime(buffer, 128, "%H:%M:%S", localtime(&curTime.tv_sec));
        char currentTime[128] = "";
        snprintf(currentTime, 128, "%s:%ld", buffer, (long)curTime.tv_usec);
        (*out) << GetLogColor(level) << GetLogAbbrev(level) << "[" << currentTime << "]\033[0m";
    }

    DoLog(level, format, ap);
}

void Logger::DoLog(LogLevel level, const char* format, va_list ap)
{
    char buffer[1024];
    vsnprintf(buffer, 1024, format, ap);
    (*out) << buffer << std::endl;
    if (level == LogLevel::FATAL) exit(EXIT_FAILURE);
}

inline int diff_ms(timeval& t1, timeval& t2)
{
    return (((t1.tv_sec - t2.tv_sec) * 1000000) +
            (t1.tv_usec - t2.tv_usec))/1000;
}

EventLog::EventLog(const char* function) : func(function)
{
    gettimeofday(&startTime, NULL);
    VerboseLog("start: %s", func);
}

EventLog::~EventLog()
{
    timeval endTime;
    gettimeofday(&endTime, NULL);
    VerboseLog("end: %s time: %dms", func, diff_ms(endTime, startTime));
}
