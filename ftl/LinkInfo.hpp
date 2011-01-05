/*
 * LinkInfo.hpp -- basic linkage information
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_LINKINFO_HPP
#define FTL_LINKINFO_HPP

#include "String.hpp"

namespace ftl
{

class LinkInfo: public Instance
{
public:
	LinkInfo(void* addr = 0);
	
	String libraryPath() const;
	String symbolName() const;
	void* baseAddress() const;
	void* symbolAddress() const;
	
	int majorVersion() const;
	int minorVersion() const;
	int patchLevel() const;
	
private:
	String libraryPath_;
	void* baseAddress_;
	String symbolName_;
	void* symbolAddress_;
	
	int majorVersion_;
	int minorVersion_;
	int patchLevel_;
};

} // namespace ftl

#endif // FTL_LINKINFO_HPP
