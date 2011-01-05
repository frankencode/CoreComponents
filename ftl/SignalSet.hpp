/*
 * SignalSet.hpp -- set of signals
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
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
	SignalSet();
	
	void clear();
	void fill();
	void add(int signal);
	void del(int signal);
	
	bool contains(int signal) const;
	
	sigset_t* rawSet();
	
private:
	sigset_t rawSet_;
};

} // namespace ftl

#endif // FTL_SIGNALSET_HPP
