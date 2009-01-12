#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_FASTTIME_H
#include <fasttime.h>
#endif

#ifdef HAVE_TSCI2_H
#include <tsci2.h>
#endif

#include <limits.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define UNUSED_ARGUMENT(x) (void)x

#include "svnversion.h"

#ifdef SVNVERSION
#define FLOWGRIND_VERSION SVNVERSION
#elif defined PACKAGE_VERSION
#define FLOWGRIND_VERSION PACKAGE_VERSION
#else
#define FLOWGRIND_VERSION "(n/a)"
#endif

#define DEFAULT_LISTEN_PORT	5999

#define ERR_FATAL	0
#define ERR_WARNING	1

#define ASSIGN_MIN(s, c) if ((s)>(c)) (s) = (c)
#define ASSIGN_MAX(s, c) if ((s)<(c)) (s) = (c)

void error(int errcode, const char *fmt, ...);

#endif
