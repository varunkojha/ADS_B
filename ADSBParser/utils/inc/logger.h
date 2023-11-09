#ifndef _LOGGER_H
#define _LOGGER_H

#include <stdio.h>
#include <stdarg.h>

#define LOG_USE_COLOR

typedef void (*log_LockFn)(void *udata, int lock);

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

#define log_trace(...) debug(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) debug(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)  debug(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  debug(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) debug(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) debug(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

void log_set_udata(void *udata);
void log_set_lock(log_LockFn fn);
void log_set_fp(FILE *fp);
void log_set_level(int level);
void log_set_quiet(int enable);

void debug(int level, const char *file, int line, const char *fmt, ...);

#endif