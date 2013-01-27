 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_SIGNALSET_HPP
#define FTL_SIGNALSET_HPP

#include <signal.h>
#include "Instance.hpp"

namespace ftl
{

class SignalSet: public Instance, public sigset_t
{
public:
	inline static O<SignalSet> createEmpty() {
		return new SignalSet(Empty);
	}
	inline static O<SignalSet> createFull() {
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
