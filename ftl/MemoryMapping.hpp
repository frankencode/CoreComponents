/*
 * MemoryMapping.hpp -- file backed / virtual memory mappings
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_MEMORYMAPPING_HPP
#define FTL_MEMORYMAPPING_HPP

#include <sys/types.h>
#include <sys/mman.h>
#include "atoms"
#include "File.hpp"

namespace ftl
{

class MemoryMapping: public Instance
{
public:
	enum Type {
		Shared  = MAP_SHARED,
		Private = MAP_PRIVATE
	};
	
	enum ProtectionFlags {
		Read    = PROT_READ,
		Write   = PROT_WRITE,
		Execute = PROT_EXEC
	};
	
	MemoryMapping(Ref<File> file, off_t offset, size_t length, int type = Shared);
	MemoryMapping(size_t length, int protection = Read|Write, int type = Shared);
	~MemoryMapping();
	
	void* start() const;
	int length() const;
	
	static int pageSize();
	
private:
	void* start_;
	int length_;
};

} // namespace ftl

#endif // FTL_MEMORYMAPPING_HPP
