#ifndef LOGGER_LIBRARY_H
#define LOGGER_LIBRARY_H

#define BUFFER_SIZE 256

#ifdef __AVR__
#include  <Arduino.h>
#include <stdio.h>
typedef void (*Callback)(const char*);
#endif

#ifdef   __linux__
#define  PC_DEVICE
#include <string>
#include  <mutex>
#include <functional>
using Callback = std::function<void(const char *)>;
using namespace std;

#endif

enum class LogLevel {
    INFO,
    DEBUG,
    WARNING,
    ERROR,
    FATAL
};

template<size_t bufferSize>
class Logger {
    public:
    Logger() =  delete;
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;
    static void log(LogLevel level,const char* content);
    static void setWriteCallback(const Callback &writeCallback) ;
    template<typename... Args>
    static void logF(const LogLevel level,const char *format, Args... args)
    {
        char buffer[BUFFER_SIZE];
        const int size =  snprintf(buffer,sizeof(buffer), format,args...);
        if(size < 0) {
            log(LogLevel::ERROR, "Format error in log message");
            return;
        }
        if(static_cast<size_t>(size) >= sizeof(buffer)) {
            log(LogLevel::ERROR, "Log message truncated");
            // Option : utiliser un buffer dynamic pour les messages longs
        }
        log(level, buffer);
    }

private:
    static Callback _writeCallback;
    const static char * LogLevelToStr(LogLevel level) ;
#ifdef PC_DEVICE
    static std::mutex _mutex;
#endif

};

template<size_t bufferSize>
Callback Logger<bufferSize>::_writeCallback = nullptr;

#ifdef PC_DEVICE
template<size_t bufferSize>
std::mutex Logger<bufferSize>::_mutex = std::mutex();
#endif

template<size_t bufferSize>
void Logger<bufferSize>::log(const LogLevel level, const char *content) {
    #ifdef PC_DEVICE
        std::lock_guard<std::mutex> lock(_mutex);
    #endif

    if (content == nullptr) {
        return;
    }
    char tempBuffer[bufferSize];
    int len = snprintf(tempBuffer,bufferSize,"[%s] %s",LogLevelToStr(level),content);

    if (len < 0 || len >= bufferSize) //error de format
        return;

    if (_writeCallback != nullptr) {
        _writeCallback(static_cast<const char *>(tempBuffer));
    }
}

template<size_t bufferSize>
void Logger<bufferSize>::setWriteCallback(const Callback &writeCallback) {
    if (writeCallback != nullptr) {
        _writeCallback = writeCallback;
    }
}

template<size_t bufferSize>
const char * Logger<bufferSize>::LogLevelToStr(const LogLevel level) {
    switch (level) {
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
    }
}

#endif // LOGGER_LIBRARY_H