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
#include "Ref.hpp"

namespace ftl
{

class SignalSet: public Instance, public sigset_t
{
public:
	inline static Ref<SignalSet, Owner> createEmpty() {
		return new SignalSet(Empty);
	}
	inline static Ref<SignalSet, Owner> createFull() {
		return new SignalSet(Full);
	}

	void insert(int signal);
	void remove(int signal);

	bool contains(int signal) const;

private:
	enum { Empty, Full };
	SignalSet(int preset);
	sigset_t rawSet_;
};

} // namespace ftl

#endif // FTL_SIGNALSET_HPP
