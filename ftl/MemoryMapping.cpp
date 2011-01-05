/*
 * MemoryMapping.cpp -- file backed / virtual memory mappings
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include <unistd.h> // sysconf
#include "MemoryMapping.hpp"

namespace ftl
{

MemoryMapping::MemoryMapping(Ref<File> file, off_t offset, size_t length, int type)
	: length_(length)
{
	if (!file->isOpen())
		FTL_THROW(StreamSemanticException, "File needs to be opened, before it can be mapped.");
	
	int prot = 0;
	if ((file->openFlags() & File::Read) != 0)
		prot |= Read;
	if ((file->openFlags() & File::Write) != 0)
		prot |= Write;
	if ((file->openFlags() & File::Execute) != 0)
		prot |= Execute;
	
	start_ = ::mmap(NULL, length, prot, type, file->fd(), offset);
	if (start_ == MAP_FAILED)
		FTL_SYSTEM_EXCEPTION;
}

MemoryMapping::MemoryMapping(size_t length, int prot, int type)
	: length_(length)
{
	start_ = ::mmap(NULL /*addr*/, length /*length*/, prot,
		#ifdef MAP_ANONYMOUS
			MAP_ANONYMOUS
		#else
			MAP_ANON
		#endif
		|type, -1 /*fd*/, 0 /*offset*/
	);
	if (start_ == MAP_FAILED)
		FTL_SYSTEM_EXCEPTION;
}

MemoryMapping::~MemoryMapping()
{
	if (::munmap((char* /*suncc issue*/)start_, length_) == -1)
		FTL_SYSTEM_EXCEPTION;
}

void* MemoryMapping::start() const { return start_; }
int MemoryMapping::length() const { return length_; }

int MemoryMapping::pageSize()
{
	return sysconf(_SC_PAGESIZE);
}

} // namespace ftl
