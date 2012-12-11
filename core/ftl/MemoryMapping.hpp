/*
 * MemoryMapping.hpp -- file backed / virtual memory mappings
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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

	void *start() const;
	size_t length() const;

	static size_t pageSize();

private:
	void *start_;
	size_t length_;
};

} // namespace ftl

#endif // FTL_MEMORYMAPPING_HPP
