/*
 * console-logging.h
 *
 *  Created on: Jan 5, 2013
 *      Author: ionut
 */

#include "console-logging.h"

#include <iostream>




ConsoleLoggerMethod& ConsoleLoggerMethod::getInstance()
{
	static ConsoleLoggerMethod instance;
	return instance;
}

void ConsoleLoggerMethod::operator() ( const LogLevel level, const std::string& file, const unsigned int line, const std::string& function, const std::string& message ) const
{
	switch( level )
	{
		case LevelVerbose:
			std::cout << "VERBOSE  : " << file.c_str() <<":"<< line << " " << function.c_str() << "(): " << message.c_str() << endl;
		break;
		case LevelDebug:
			std::cout << "DEBUG  : " << file.c_str() <<":"<< line << " " << function.c_str() << "(): " << message.c_str() << endl;
		break;
		case LevelInfo:
			std::cout << "INFO  : " << file.c_str() <<":"<< line << " " << function.c_str() << "(): " << message.c_str() << endl;
		break;
		case LevelWarn:
			std::cout << "WARN  : " << file.c_str() <<":"<< line << " " << function.c_str() << "(): " << message.c_str() << endl;
		break;
		case LevelError:
			std::cout << "ERROR  : " << file.c_str() <<":"<< line << " " << function.c_str() << "(): " << message.c_str() << endl;
		break;
		case LevelCritical:
			std::cout << "CRITICAL  : " << file.c_str() <<":"<< line << " " << function.c_str() << "(): " << message.c_str() << endl;
		break;
		case LevelFatal:
			std::cout << "FATAL  : " << file.c_str() <<":"<< line << " " << function.c_str() << "(): " << message.c_str() << endl;
		break;
	}
}
