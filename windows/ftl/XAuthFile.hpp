 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_XAUTHFILE_HPP
#define FTL_XAUTHFILE_HPP

#include <arpa/inet.h>
#include <ftl/string.hpp>
#include <ftl/List.hpp>

namespace ftl
{

class XAuthRecord: public Instance
{
public:
	int family;
	string host;
	int display;
	string protocol;
	string data;
};

typedef List< Ref<XAuthRecord> > XAuthRecords;

class XAuthFile: public Instance
{
public:
	XAuthFile(string path = "");
	inline string path() const { return path_; }
	inline XAuthRecords *records() const { return records_; }

private:
	string path_;
	Ref<XAuthRecords> records_;
};

} // namespace ftl

#endif // FTL_XAUTHFILE_HPP
