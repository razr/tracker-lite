/*
 * logging.h
 *
 *  Created on: Feb 8, 2013
 */

#ifndef LOGGING_H_
#define LOGGING_H_


#include <list>
#include <string>
#include <glib.h>


namespace Logging
{
enum LogLevel
{
	LevelVerbose = 0,
	LevelDebug,
	LevelInfo,
	LevelWarn,
	LevelError,
	LevelCritical,
	LevelFatal
};

class LoggingMethod
{
public:
	virtual void operator() ( const LogLevel level, const std::string& file, const unsigned int line, const std::string& function, const std::string& message ) const = 0;
};

class LoggerDelegate
{
	LoggingMethod & m_method;
public:
	LoggerDelegate( LoggingMethod & method )
	: m_method( method )
	{

	}
	void log( const LogLevel level, const std::string& file, const unsigned int line, const std::string& function, const std::string &message)
	{
		m_method( level, file, line, function, message);
	}

};

class Logger
{
	std::list<LoggerDelegate> m_delegates;
	LogLevel m_logLevel;
	Logger();
	GMutex *m_mutex;
public:
	void setLogLevel( LogLevel level );
	const LogLevel getLogLevel();
	static Logger & getInstance();
	void addLoggerDelegate( LoggerDelegate delegate );
	void log( const LogLevel level, const std::string& file, const unsigned int line, const std::string& function, const std::string &format, ... );
};
}


#define LOG_VERBOSE 	Logging::LevelVerbose
#define LOG_DEBUG   	Logging::LevelDebug
#define	LOG_INFO		Logging::LevelInfo
#define	LOG_WARN		Logging::LevelWarn
#define LOG_ERROR		Logging::LevelError
#define LOG_CRITICAL 	Logging::LevelCritical
#define LOG_FATAL		Logging::LevelFatal

#define LOG(__LOG_LEVEL__, __FORMAT__ , ... ) \
			Logging::Logger::getInstance().log( __LOG_LEVEL__, \
												__FILE__ , \
												__LINE__ , \
												__FUNCTION__, \
												__FORMAT__ , \
												__VA_ARGS__ )


#endif /* LOGGING_H_ */
