#ifndef LOGGER_LIBRARY_H
#define LOGGER_LIBRARY_H

// -----------------------------------------------------------------------------
// PLATFORM DETECTION
// -----------------------------------------------------------------------------

#if defined(__AVR__) || defined(ARDUINO)
    #define MCU_DEVICE
    #include <Arduino.h>
    typedef void (*Callback)(const char*);
#elif defined(__linux__) || defined(__unix__)
#define PC_DEVICE
#include <cstdio>
#include <mutex>
#include <functional>
using Callback = std::function<void(const char *)>;
#elif defined(_WIN32)
#define PC_DEVICE
#include <cstdio>
#include <mutex>
#include <functional>
using Callback = std::function<void(const char *)>;
#elif defined(__APPLE__)
#define PC_DEVICE
#include <cstdio>
#include <mutex>
#include <functional>
using Callback = std::function<void(const char *)>;
#elif defined(STM32F1xx) || defined(STM32F4xx) || defined(STM32H7xx)
#define MCU_DEVICE
#include "stm32f4xx_hal.h"
typedef void (*Callback)(const char*);
#else
#error "Unsupported platform"
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
    static void setWriteCallback(const Callback &cb) ;
    template<typename... Args>
    static void logF(const LogLevel level,const char *format, Args... args)
    {
        char buffer[bufferSize];
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
    static constexpr const char * LogLevelToStr(LogLevel level) ;
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
    else {
#ifdef ARDUINO
        Serial.println(tempBuffer);
#else
        printf("%s\n", tempBuffer);
#endif
    }
}

template<size_t bufferSize>
void Logger<bufferSize>::setWriteCallback(const Callback &cb) {
    _writeCallback = cb;
}

template<size_t bufferSize>
constexpr const char * Logger<bufferSize>::LogLevelToStr(const LogLevel level) {
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