/** @file Logger.h
 *  @brief Generates program logging information and csv pattern data
 *
 *  Used for general program logging and generating pattern data output
 *
 *  @author Peter J. Morley (pmorley)
 */
#pragma once
#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <sstream>

/** Paths to logger and csv files are different based on OS */
const std::string LOGGERPATH = "";
const bool cmdEnabled        = true;
const bool disableLogging    = true;

enum class LOG_LEVEL {
    FATAL = 100,
    ERR   = 200,
    WARN  = 300,
    INFO  =	400,
    DEBUG =	500,
    TRACE =	600
};

#define LOGGERCLI "-l"

#define LOG_TRACE(...) Logger::writeLog(__FILE__, __func__, __LINE__, LOG_LEVEL::TRACE, __VA_ARGS__)
#define LOG_DEBUG(...) Logger::writeLog(__FILE__, __func__, __LINE__, LOG_LEVEL::DEBUG, __VA_ARGS__)
#define LOG_INFO(...)  Logger::writeLog(__FILE__, __func__, __LINE__, LOG_LEVEL::INFO,  __VA_ARGS__)
#define LOG_WARN(...)  Logger::writeLog(__FILE__, __func__, __LINE__, LOG_LEVEL::WARN,  __VA_ARGS__)
#define LOG_ERR(...)   Logger::writeLog(__FILE__, __func__, __LINE__, LOG_LEVEL::ERR,   __VA_ARGS__)
#define LOG_FATAL(...) Logger::writeLog(__FILE__, __func__, __LINE__, LOG_LEVEL::FATAL, __VA_ARGS__)

class Logger
{
public:
    template<typename... Args>
    static void writeLog(const char* file,
                         const char* func,
                         int         line,
                         LOG_LEVEL   level,
                         Args...     args) {

        if (!verbosity     ||
            disableLogging ||
            level > logLevel) {
            return;
        }

        std::stringstream stream;
        writeLog(stream,
                 args...);

        auto buffer = stream.str();
        dumpLog(file,
                func,
                line,
                level,
                buffer);
    }

    static void           setLogLevel(std::string log_level);
    static void           closeLog();
    static int            verbosity;
    static LOG_LEVEL      logLevel;

private:
    static std::ofstream* outputFile;
    static std::mutex*    logMutex;
    static std::string    getPID();

    template<typename T>
    static void writeLog(std::stringstream& stream,
                         T                  streamable) {
        stream << streamable;
    }

    template<typename T, typename... Args>
    static void writeLog(std::stringstream& stream,
                         T                  streamable,
                         Args...            args) {

        writeLog(stream, streamable);
        writeLog(stream, args...);
    }

    static void dumpLog(const char*        file,
                        const char*        func,
                        int                line,
                        LOG_LEVEL          level,
                        const std::string& buffer);
};