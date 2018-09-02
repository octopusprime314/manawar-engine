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

const bool cmdEnabled = true;
const bool disableLogging = false;

enum class LOG_LEVEL {
	FATAL = 100,
	ERR = 200,
	WARN  = 300,
	INFO  =	400,
	DEBUG =	500,
	TRACE =	600
};

class Logger
{
public:
	template<typename... Args>
	static void WriteLog(LOG_LEVEL level, Args... args) {

		if (!verbosity || disableLogging || level > debugLevel) {
			return;
		}

		std::stringstream stream;
		writeLog(stream, args...);

		auto buffer = stream.str();
		dumpLog(level, buffer);
	}

	template<typename... Args>
	static void TRACE( Args... args) {
		WriteLog(LOG_LEVEL::TRACE, args...);
	}

	template<typename... Args>
	static void DEBUG(Args... args) {
		WriteLog(LOG_LEVEL::DEBUG, args...);
	}

	template<typename... Args>
	static void INFO(Args... args) {
		WriteLog(LOG_LEVEL::INFO, args...);
	}

	template<typename... Args>
	static void WARN(Args... args) {
		WriteLog(LOG_LEVEL::WARN, args...);
	}

	template<typename... Args>
	static void FATAL(Args... args) {
		WriteLog(LOG_LEVEL::FATAL, args...);
	}

	template<typename... Args>
	static void ERR(Args... args) {
		WriteLog(LOG_LEVEL::ERR, args...);
	}

	/** @brief Closes log file by closing handle
	 *
	 *  Closes file
	 *
	 *  @return void
	 */
	static void CloseLog();

	/** sets logging on or off */
	static int verbosity;

	static LOG_LEVEL debugLevel;

private:
	/** Streams used to write and read to files */
	static std::ofstream* outputFile;

	/** Mutexes used for logging */
	static std::mutex* logMutex;

	/** Get the Process ID of the current process */
	static std::string GetPID();

	template<typename T>
	static void writeLog(std::stringstream& stream, T streamable) {
		stream << streamable << std::endl;
	}

	template<typename T, typename... Args>
	static void writeLog(std::stringstream& stream, T streamable, Args... args) {
		writeLog(stream, streamable);
		writeLog(stream, args...);
	}

	static void dumpLog(LOG_LEVEL level, const std::string& buffer);
};