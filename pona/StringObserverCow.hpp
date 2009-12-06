/*
 * StringObserverCow.hpp -- copy-on-write (cow) strategy for strings
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_STRINGOBSERVERCOW_HPP
#define PONA_STRINGOBSERVERCOW_HPP

#include "ListObserver.hpp"

namespace pona
{

class String;

class StringObserverCow: public ListObserver<Char>
{
public:
	StringObserverCow(Ref<String> string);
	
	virtual bool beforePush(int i, int n, const Char* v);
	virtual bool beforePop(int i, int n);
	virtual bool beforeWrite(int i, int n, const Char* v);

private:
	Ref<String, SetNull> string_;
};

} // namespace pona

#endif // PONA_STRINGOBSERVERCOW_HPP
