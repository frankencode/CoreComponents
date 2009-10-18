/*
 * Context.hpp -- user-level context switching
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_CONTEXT_HPP
#define PONA_CONTEXT_HPP

#include <ucontext.h>
#include "atoms"

namespace pona
{

class PONA_API Context: public ucontext_t, public Instance
{
public:
	Context();
	virtual ~Context();
	
	static void get(Context* current);
	static void set(Context* current);
	static void swap(Context* current, Context* next);
	static void make(Context* context, void (*entry)(), ssize_t stackSize = -1, ssize_t guardSize = -1, Context* next = 0);
	
	void* stack() const;
	int stackSize() const;
	int guardSize() const;
	
	static bool stackGrowsDownwards(int h = 0);
	
private:
	void* stack_;
	size_t stackSize_;
	size_t guardSize_;
};

} // namespace pona

#endif // PONA_CONTEXT_HPP
