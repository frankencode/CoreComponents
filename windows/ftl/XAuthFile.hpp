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
#include <ftl/String.hpp>
#include <ftl/List.hpp>

namespace ftl
{

class XAuthRecord: public Instance
{
public:
	int family;
	String host;
	int display;
	String protocol;
	String data;
};

typedef List< hook<XAuthRecord> > XAuthRecords;

class XAuthFile: public Instance
{
public:
	XAuthFile(String path = "");
	inline String path() const { return path_; }
	inline XAuthRecords *records() const { return records_; }

private:
	String path_;
	hook<XAuthRecords> records_;
};

} // namespace ftl

#endif // FTL_XAUTHFILE_HPP
