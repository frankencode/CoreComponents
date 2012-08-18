/*
 * SignalSet.hpp -- set of signals
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_SIGNALSET_HPP
#define FTL_SIGNALSET_HPP

#include <signal.h>
#include "atoms"

namespace ftl
{

class SignalSet: public Instance
{
public:
	inline static Ref<SignalSet, Owner> newInstance() {
		return new SignalSet;
	}

	void clear();
	void fill();
	void add(int signal);
	void del(int signal);

	bool contains(int signal) const;

	sigset_t *rawSet();

private:
	SignalSet();
	sigset_t rawSet_;
};

} // namespace ftl

#endif // FTL_SIGNALSET_HPP
