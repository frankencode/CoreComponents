/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_SIGNALSET_H
#define FKIT_SIGNALSET_H

#include <signal.h>
#include "Object.h"

namespace fkit
{

class Thread;

class SignalSet: public Object
{
public:
	inline static Ref<SignalSet> createEmpty() {
		return new SignalSet(Empty);
	}
	inline static Ref<SignalSet> createFull() {
		return new SignalSet(Full);
	}

	void insert(int signal);
	void remove(int signal);

	bool contains(int signal) const;

	inline sigset_t *rawSet() { return &rawSet_; }

private:
	friend class Thread;

	enum { Empty, Full };
	SignalSet(int preset);
	sigset_t rawSet_;
};

} // namespace fkit

#endif // FKIT_SIGNALSET_H
