/*****************************************************************************
 * logger.cpp : logging tools
 *****************************************************************************
 * Copyright (C) 2014-2015 BBC
 *
 * Authors: James P. Weaver <james.barrett@bbc.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *
 * This program is also available under a commercial proprietary license.
 * For more information, contact us at ipstudio@bbc.co.uk.
 *****************************************************************************/

#include "internal.h"
#include "logger.hpp"

#include <cstdio>
#include <ctime>
#include <cstdarg>
#include <cstdlib>

#ifdef _WIN32
int vasprintf(char **msg, const char *fmt, va_list args) {
	int size = _vscprintf(fmt, args);
	if (size <= 0)
		return size;

	*msg = (char *)malloc(size + 1);
  msg[size] = 0x00;
	return vsprintf(*msg, fmt, args);
}
#endif

VC2DecoderLoggers loggers = { NULL, NULL, NULL, NULL, NULL };

const char *loglevels[] = { "ERROR", "WARN", "INFO", "DEBUG" };

void vc2decode_init_logging(VC2DecoderLoggers l) {
	loggers = l;
}

void writelog(int level, const char *fmt, ...) {
	va_list args;
	char *msg;
	va_start(args, fmt);
	if (vasprintf(&msg, fmt, args) < 0)
		return;

	switch (level) {
	case LOG_DEBUG:
		if (loggers.debug) {
			loggers.debug(msg, loggers.opaque);
			break;
		}
	case LOG_INFO:
		if (loggers.info) {
			loggers.info(msg, loggers.opaque);
			break;
		}
	case LOG_WARN:
		if (loggers.warn) {
			loggers.warn(msg, loggers.opaque);
			break;
		}
	case LOG_ERROR:
		if (loggers.error) {
			loggers.error(msg, loggers.opaque);
			break;
		}
	default:
		fprintf(stderr, "%s: %s\n", loglevels[level], msg);
	}

	free(msg);
}
