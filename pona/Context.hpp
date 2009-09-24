#ifndef PONA_CONTEXT_HPP
#define PONA_CONTEXT_HPP

#include <ucontext.h>
#include "atoms"

namespace pona
{

class Context: public ucontext_t, public Instance
{
public:
	Context();
	virtual ~Context();
	
	static void get(Context* current);
	static void set(Context* current);
	static void swap(Context* current, Context* next);
	static void make(Context* context, void (*entry)(), int stackSize = 8192, Context* next = 0);
	
private:
	void* stack_;
};

} // namespace pona

#endif // PONA_CONTEXT_HPP
