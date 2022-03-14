//
// C++ Interface: misc.h
//
// Description:
//
//
// Author: Sven Queisser <tincan@svenqueisser.de>, (C) 2001 - 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef MISC_H
#define MISC_H

#include <string>

double vToDb(double voltage, double reference);
double dbToV(double dbs, double reference);
double stringToNum(const std::string &text);
std::string numToString(double value);
bool isNear(double a, double b);

#if HAVE_GETLINE != 1
ssize_t getdelim (char **lineptr, size_t *n, int delim, FILE *stream);
#define getline(line, size, stream) getdelim(line, size, '\n', stream)
#endif

void printMessage(const char *file, const char *function, int line, int level, const char *text);

// macros for debug messages
#define MSG_NOTICE 0
#define MSG_DEBUG 10
#define MSG_INFO  20
#define MSG_WARN  30
#define MSG_ERROR 40
#define MSG_NONE  100
#define MSG_LEVEL MSG_NOTICE

#define PRTHIS printf("%s:%d: %s\n", __FILE__, __LINE__, __FUNCTION__); fflush(stdout);
#define MSG(LVL, TEXT) (printMessage(__FILE__, __FUNCTION__, __LINE__, LVL, TEXT))
#define LOC __FILE__ ":" __LINE__ ": " __FUNCTION__ ": "

#endif // MISC_H
