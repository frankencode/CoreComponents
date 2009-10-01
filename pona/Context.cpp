/*
 * Context.cpp -- user-level context switching
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <pthread.h>
#include <unistd.h> // sysconf
#include <sys/mman.h> // mmap, munmap, mprotect
#include "Exception.hpp"
#include "ThreadFactory.hpp"
#include "Context.hpp"

namespace pona
{

Context::Context()
	: stack_(0)
{}

Context::~Context()
{
	if (stack_) {
		if (::munmap(stack_, stackSize_ + guardSize_) == -1)
			PONA_SYSTEM_EXCEPTION;
		stack_ = 0;
	}
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

void Context::make(Context* context, void (*entry)(), ssize_t stackSize, ssize_t guardSize, Context* next)
{
	if (context->stack_) {
		if (::munmap(context->stack_, context->stackSize_ + context->guardSize_) == -1)
			PONA_SYSTEM_EXCEPTION;
		context->stack_ = 0;
	}
	int pageSize = sysconf(_SC_PAGESIZE);
	if (pageSize == -1)
		PONA_SYSTEM_EXCEPTION;
	
	if (stackSize == -1) {
		stackSize = ThreadFactory().stackSize();
	}
	else if (stackSize % pageSize != 0)
		stackSize += pageSize - (stackSize % pageSize);
	
	if (guardSize != 0) {
		if (guardSize == -1)
			guardSize = ThreadFactory().guardSize();
		else if (guardSize % pageSize != 0)
			guardSize += pageSize - (guardSize % pageSize);
	}
	
	void* mapStart =
		::mmap(NULL /*addr*/, stackSize + guardSize /*length*/,
			PROT_READ|PROT_WRITE /*prot*/,
			MAP_PRIVATE
			#ifdef MAP_ANONYMOUS
				|MAP_ANONYMOUS
			#else
				|MAP_ANON
			#endif
			#ifdef MAP_GROWSDOWN
				|MAP_GROWSDOWN
			#endif
			/*flags*/
			,
			-1 /*fd*/, 0 /*offset*/
		);
	if (mapStart == MAP_FAILED)
		PONA_SYSTEM_EXCEPTION;
	
	bool growsDown = stackGrowsDownwards();
	
	if (guardSize != 0) {
		if (growsDown) {
			if (::mprotect(mapStart, guardSize, PROT_NONE) == -1)
				PONA_SYSTEM_EXCEPTION;
		}
		else {
			if (::mprotect((void*)((char*)mapStart + stackSize), guardSize, PROT_NONE) == -1)
				PONA_SYSTEM_EXCEPTION;
		}
	}
	
	context->stack_ = (growsDown) ? (void*)((char*)mapStart + guardSize) : mapStart;
	context->stackSize_ = stackSize;
	context->guardSize_ = guardSize;
	
	context->uc_stack.ss_sp = context->stack_;
	context->uc_stack.ss_size = context->stackSize_;
	context->uc_link = next;
	::makecontext(context, entry, 0);
}

void* Context::stack() const { return stack_; }
int Context::stackSize() const { return stackSize_; }
int Context::guardSize() const { return guardSize_; }

bool Context::stackGrowsDownwards(int h1)
{
	int h2;
	return &h2 < &h1;
}

} // namespace pona
