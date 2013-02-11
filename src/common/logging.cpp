/*
 * logging.cpp
 *
 *  Created on: Feb 8, 2013
 *      Author: ionut
 */


#include "logging.h"

#include <stdio.h>
#include <stdarg.h>



using namespace Logging;

Logger::Logger()
	: m_logLevel( LevelWarn )
{
	m_mutex = g_mutex_new();
}

void Logger::setLogLevel( LogLevel level )
{
	m_logLevel = level;
}
const LogLevel Logger::getLogLevel()
{
	return m_logLevel;
}
Logger & Logger::getInstance()
{
	static Logger instance;
	return instance;
}

void Logger::addLoggerDelegate( LoggerDelegate delegate )
{
	m_delegates.push_back( delegate );
}

void Logger::log( const LogLevel level, const std::string& file, const unsigned int line, const std::string& function, const std::string &format, ... )
{

	if( level < m_logLevel )
		return;

	char message [40960];

	g_mutex_lock( m_mutex );
		va_list args;
		va_start (args, format.c_str() );
		vsprintf (message, format.c_str(), args);
		va_end (args);


	for( std::list<LoggerDelegate>::iterator iter = m_delegates.begin();  iter != m_delegates.end(); ++ iter )
	{
		iter->log( level, file, line, function, message );
	}

	g_mutex_unlock( m_mutex );

}



