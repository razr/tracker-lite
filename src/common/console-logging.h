/*
 * console logging.h
 *
 *  Created on: Feb 8, 2013
 *      Author: ionut
 */

#ifndef CONSOLE_LOGGING_H_
#define CONSOLE_LOGGING_H_

#include "logging.h"

using namespace Logging;
using namespace std;

class ConsoleLoggerMethod : public LoggingMethod
{
public:
	void operator() ( const LogLevel level, const std::string& file, const unsigned int line, const std::string& function, const std::string& message ) const;
	static ConsoleLoggerMethod& getInstance();
};

#endif /* CONSOLE_LOGGING_H */
