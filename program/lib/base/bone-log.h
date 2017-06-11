// Nguyen Hai Duong
// Oct 29 2016

#ifndef BONE_LOG_H
#define BONE_LOG_H

#include <iostream>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <string>
#include <fstream>
#include <inttypes.h>	// int64_t
#include <stdlib.h>		// exit()
#include <errno.h>
#include <signal.h> // signal

#include "bone-config.h" // configuration for system

namespace BONE {

// #define LOGFILE "../log/root.log"  // define a log-file for system. its may be redefined to log in specific log-file in each prog.
// #define ABSLOGFILE "../log/root.log"

extern const char *g_program_name;

char *DateAndTime(time_t rawtime);

struct LogMessageEnvelope {
  enum Severity {
    Error = -2,
    Warning = -1,
    Info = 0,
    Verbose = 1
  };
  // An 'enum Severity' value, or a positive number indicating verbosity level.
  int severity;
  const char *func;
  const char *file;
  int logLevel;
  int32_t line;
};

class MessageLogger {

public:

	MessageLogger(LogMessageEnvelope::Severity severity, const char *func,
	              const char *file, int32_t line);
	~MessageLogger();
	inline std::ostream &stream() { return ss_; }

private:
	LogMessageEnvelope envelope_;
	std::ostringstream ss_;
  
};

#define BONE_ERROR \
	MessageLogger(LogMessageEnvelope::Error, \
                         __FUNCTION__, __FILE__, __LINE__).stream()
#define BONE_WARN \
	MessageLogger(LogMessageEnvelope::Warning, \
                         __FUNCTION__, __FILE__, __LINE__).stream()
#define BONE_LOG \
	MessageLogger(LogMessageEnvelope::Info, \
                         __FUNCTION__, __FILE__, __LINE__).stream()
#define BONE_VLOG \
	MessageLogger(LogMessageEnvelope::Verbose, \
                         __FUNCTION__, __FILE__, __LINE__).stream()

} // end of namespace TREE

#endif