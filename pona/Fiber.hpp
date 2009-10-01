/*
 * Fiber.hpp -- fibers, also know as coroutines
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_FIBER_HPP
#define PONA_FIBER_HPP

#include "atoms"
#include "Context.hpp"

namespace pona
{

class FiberInitializer
{
public:
	FiberInitializer();
	~FiberInitializer();
private:
	static int count_;
};

namespace { FiberInitializer fiberInitializer_; }

class Fiber: public Context
{
public:
	Fiber(int stackSize = -1, int guardSize = -1, Fiber* next = 0);
	
	void resume();
	
protected:
	void yield();
	void transferTo(Fiber* target);
	
	virtual void run() = 0;
	
private:
	static void bootstrap();
	Context saved_;
};

} // namespace pona

#endif // PONA_FIBER_HPP
