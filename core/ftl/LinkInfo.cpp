 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include <dlfcn.h>
#include "File.hpp"
#include "LinkInfo.hpp"

int ftl_hook;

namespace ftl
{

LinkInfo::LinkInfo(void *addr)
	: baseAddress_(0),
	  symbolAddress_(0),
	  majorVersion_(0),
	  minorVersion_(0),
	  patchLevel_(0)
{
	if (!addr) addr = (void *)&ftl_hook;
	Dl_info info;
	if (::dladdr(addr, &info)) {
		libraryPath_ = File::resolve(info.dli_fname);
		symbolName_ = info.dli_sname;
		baseAddress_ = info.dli_fbase;
		symbolAddress_ = info.dli_saddr;
		hook<StringList> parts = libraryPath_->split(".");
		if (parts->length() > 3) {
			int i = parts->length() - 1;
			if (parts->at(i) == "dylib") --i;
			majorVersion_ = parts->at(i-2)->toInt();
			minorVersion_ = parts->at(i-1)->toInt();
			patchLevel_   = parts->at(i)->toInt();
		}
	}
}

String LinkInfo::libraryPath() const { return libraryPath_; }
String LinkInfo::symbolName() const { return symbolName_; }
void *LinkInfo::baseAddress() const { return baseAddress_; }
void *LinkInfo::symbolAddress() const { return symbolAddress_; }
int LinkInfo::majorVersion() const { return majorVersion_; }
int LinkInfo::minorVersion() const { return minorVersion_; }
int LinkInfo::patchLevel() const { return patchLevel_; }

} // namespace ftl
