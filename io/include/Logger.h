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

/** Paths to logger and csv files are different based on OS */
const std::string LOGGERPATH = "../../log/";

const bool cmdEnabled = true;
const bool disableLogging = false;

class Logger
{
public:

    //This is terrible logging because it logs the number of args - 1
    //I needed a way to pass though variadic arguments and it turned out poorly
    template<typename T, typename... Args>
    static void WriteLog(T streamable, Args... args) {
        
        if (!verbosity || disableLogging) {
            return;
        }
        std::stringstream stream;
        writeLog(stream, streamable, args...);
    }

    template<typename T, typename... Args>
    static void writeLog(std::stringstream& stream, T streamable, Args... args) {
       
        stream << streamable;
        writeLog(stream, args...);
        auto buffer = stream.str();
        dumpLog(buffer);
    }

    template<typename T>
    static void writeLog(std::stringstream& stream, T streamable) {
        stream << streamable << std::endl;
    }

    static void dumpLog(const std::string& buffer);

	/** @brief Closes log file by closing handle
	 *
	 *  Closes file
	 *
	 *  @return void
	 */
	static void CloseLog();

	/** sets logging on or off */
	static int verbosity;

private:
	/** Streams used to write and read to files */
	static std::ofstream* outputFile;

	/** Mutexes used for logging */
	static std::mutex* logMutex;

	/** Get the Process ID of the current process */
	static std::string GetPID();
};