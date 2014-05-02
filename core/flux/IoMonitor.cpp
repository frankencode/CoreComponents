/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <sys/time.h>
#include <math.h> // modf
#include "exceptions.h"
#include "IoMonitor.h"

namespace flux
{

IoSet *IoMonitor::readyRead()
{
	if (!readyRead_) readyRead_ = IoSet::create();
	return readyRead_;
}

IoSet *IoMonitor::readyWrite()
{
	if (!readyWrite_) readyWrite_ = IoSet::create();
	return readyWrite_;
}

IoSet *IoMonitor::readyExcept()
{
	if (!readyExcept_) readyExcept_ = IoSet::create();
	return readyExcept_;
}

void IoMonitor::reset()
{
	if (readyRead_) readyRead_->clear();
	if (readyWrite_) readyWrite_->clear();
	if (readyExcept_) readyExcept_->clear();
}

int IoMonitor::wait(double interval)
{
	int sz = 0;
	fd_set *rr = 0, *rw = 0, *re = 0;
	if (readyRead_) {
		if (readyRead_->bitSize() > 0) {
			rr = readyRead_->bitSet();
			sz = readyRead_->bitSize();
		}
	}
	if (readyWrite_) {
		if (readyWrite_->bitSize() > 0) {
			rw = readyWrite_->bitSet();
			if (readyWrite_->bitSize() > sz)
				sz = readyWrite_->bitSize();
		}
	}
	if (readyExcept_) {
		if (readyExcept_->bitSize() > 0) {
			re = readyExcept_->bitSet();
			if (readyExcept_->bitSize() > sz)
				sz = readyExcept_->bitSize();
		}
	}

	timeval *tv = 0;
	timeval h;
	if (interval >= 0) {
		tv = &h;
		double sec = 0;
		tv->tv_usec = modf(interval, &sec) * 1e6;
		tv->tv_sec = sec;
	}

	int ret = ::select(sz, rr, rw, re, tv);
	if (ret == -1) {
		if (errno == EINTR) throw Interrupt();
		FLUX_SYSTEM_DEBUG_ERROR(errno);
	}

	return ret;
}

} // namespace flux
