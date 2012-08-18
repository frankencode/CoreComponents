/*
 * LinkInfo.hpp -- basic linkage information
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_LINKINFO_HPP
#define FTL_LINKINFO_HPP

#include "String.hpp"

namespace ftl
{

class LinkInfo: public Instance
{
public:
	inline static Ref<LinkInfo, Owner> newInstance(void *addr = 0) {
		return new LinkInfo(addr);
	}

	String libraryPath() const;
	String symbolName() const;
	void *baseAddress() const;
	void *symbolAddress() const;

	int majorVersion() const;
	int minorVersion() const;
	int patchLevel() const;

private:
	LinkInfo(void *addr);

	String libraryPath_;
	void *baseAddress_;
	String symbolName_;
	void *symbolAddress_;

	int majorVersion_;
	int minorVersion_;
	int patchLevel_;
};

} // namespace ftl

#endif // FTL_LINKINFO_HPP
