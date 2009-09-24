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
	Fiber(int stackSize = 8192, Fiber* next = 0);
	virtual ~Fiber();
	
	void resume();
	void transferTo(Fiber* target);
	
protected:
	void yield();
	
	virtual void run() = 0;
	
private:
	static void bootstrap();
	Context saved_;
};

} // namespace pona

#endif // PONA_FIBER_HPP
