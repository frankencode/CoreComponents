/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_IOMONITOR_H
#define FLUX_IOMONITOR_H

#include "IoSet.h"

namespace flux
{

class IoMonitor: public Object
{
public:
	inline static Ref<IoMonitor> create() { return new IoMonitor; }

	IoSet *readyRead();
	IoSet *readyWrite();
	IoSet *readyExcept();

	inline IoSet *readyAccept() { return readyRead(); }

	int wait(double interval = -1);
	void reset();

protected:
	IoMonitor() {}
	Ref<IoSet> readyRead_;
	Ref<IoSet> readyWrite_;
	Ref<IoSet> readyExcept_;
};

} // namespace flux

#endif // FLUX_IOMONITOR_H
