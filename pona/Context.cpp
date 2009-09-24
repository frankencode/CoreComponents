#include "Exception.hpp"
#include "Context.hpp"

namespace pona
{

Context::Context()
	: stack_(0)
{}

Context::~Context()
{
	if (stack_) { ::free(stack_); stack_ = 0; }
}

void Context::get(Context* current)
{
	if (::getcontext(current) == -1)
		PONA_SYSTEM_EXCEPTION;
}

void Context::set(Context* current)
{
	if (::setcontext(current) == -1)
		PONA_SYSTEM_EXCEPTION;
}

void Context::swap(Context* current, Context* next)
{
	if (::swapcontext(current, next) == -1)
		PONA_SYSTEM_EXCEPTION;
}

void Context::make(Context* context, void (*entry)(), int stackSize, Context* next)
{
	if (context->stack_) ::free(context->stack_);
	context->stack_ = ::malloc(stackSize);
	if (!context->stack_)
		PONA_SYSTEM_EXCEPTION;
	context->uc_stack.ss_sp = context->stack_;
	context->uc_stack.ss_size = stackSize;
	context->uc_link = next;
	::makecontext(context, entry, 0);
}

} // namespace pona
