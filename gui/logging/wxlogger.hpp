#ifndef WXLOGGER_HPP
#define WXLOGGER_HPP

#include "logger.hpp"

class LoggerWx : public AbstractLogger
{
    public:
        virtual void Log(LogLevel level, const char* format, va_list ap);
};

#endif
