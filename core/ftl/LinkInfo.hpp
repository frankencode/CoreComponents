 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_LINKINFO_HPP
#define FTL_LINKINFO_HPP

#include "String.hpp"

namespace ftl
{

class LinkInfo: public Instance
{
public:
	inline static hook<LinkInfo> create(void *addr = 0) {
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
